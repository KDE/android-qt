/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qapplication.h"
#include "qevent.h"
#include "qbitmap.h"
#include "private/qsoftkeymanager_p.h"
#include "private/qobject_p.h"
#include "private/qsoftkeymanager_common_p.h"

#ifdef Q_WS_S60
#include "private/qsoftkeymanager_s60_p.h"
#endif

#ifndef QT_NO_SOFTKEYMANAGER
QT_BEGIN_NAMESPACE

QSoftKeyManager *QSoftKeyManagerPrivate::self = 0;

const char *QSoftKeyManager::standardSoftKeyText(StandardSoftKey standardKey)
{
    const char *softKeyText = 0;
    switch (standardKey) {
    case OkSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Ok");
        break;
    case SelectSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Select");
        break;
    case DoneSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Done");
        break;
    case MenuSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Options");
        break;
    case CancelSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Cancel");
        break;
    default:
        break;
    };

    return softKeyText;
}

QSoftKeyManager *QSoftKeyManager::instance()
{
    if (!QSoftKeyManagerPrivate::self)
        QSoftKeyManagerPrivate::self = new QSoftKeyManager;

    return QSoftKeyManagerPrivate::self;
}

QSoftKeyManager::QSoftKeyManager() :
#ifdef Q_WS_S60
    QObject(*(new QSoftKeyManagerPrivateS60), 0)
#else
    QObject(*(new QSoftKeyManagerPrivate), 0)
#endif
{
}

QAction *QSoftKeyManager::createAction(StandardSoftKey standardKey, QWidget *actionWidget)
{
    const char* text = standardSoftKeyText(standardKey);
    QAction *action = new QAction(QSoftKeyManager::tr(text), actionWidget);
    QAction::SoftKeyRole softKeyRole = QAction::NoSoftKey;
    switch (standardKey) {
    case MenuSoftKey: // FALL-THROUGH
        action->setProperty(MENU_ACTION_PROPERTY, QVariant(true)); // TODO: can be refactored away to use _q_action_menubar
    case OkSoftKey:
    case SelectSoftKey:
    case DoneSoftKey:
        softKeyRole = QAction::PositiveSoftKey;
        break;
    case CancelSoftKey:
        softKeyRole = QAction::NegativeSoftKey;
        break;
    }
    action->setSoftKeyRole(softKeyRole);
    return action;
}

/*! \internal

  Creates a QAction and registers the 'triggered' signal to send the given key event to
  \a actionWidget as a convenience.

*/
QAction *QSoftKeyManager::createKeyedAction(StandardSoftKey standardKey, Qt::Key key, QWidget *actionWidget)
{
#ifndef QT_NO_ACTION
    QScopedPointer<QAction> action(createAction(standardKey, actionWidget));

    connect(action.data(), SIGNAL(triggered()), QSoftKeyManager::instance(), SLOT(sendKeyEvent()));
    connect(action.data(), SIGNAL(destroyed(QObject*)), QSoftKeyManager::instance(), SLOT(cleanupHash(QObject*)));
    QSoftKeyManager::instance()->d_func()->keyedActions.insert(action.data(), key);
    return action.take();
#endif //QT_NO_ACTION
}

void QSoftKeyManager::cleanupHash(QObject *obj)
{
    Q_D(QSoftKeyManager);
    QAction *action = qobject_cast<QAction*>(obj);
    d->keyedActions.remove(action);
}

void QSoftKeyManager::sendKeyEvent()
{
    Q_D(QSoftKeyManager);
    QAction *action = qobject_cast<QAction*>(sender());

    if (!action)
        return;

    Qt::Key keyToSend = d->keyedActions.value(action, Qt::Key_unknown);

    if (keyToSend != Qt::Key_unknown)
        QApplication::postEvent(action->parentWidget(),
                                new QKeyEvent(QEvent::KeyPress, keyToSend, Qt::NoModifier));
}

void QSoftKeyManager::updateSoftKeys()
{
    QEvent *event = new QEvent(QEvent::UpdateSoftKeys);
    QApplication::postEvent(QSoftKeyManager::instance(), event);
}

bool QSoftKeyManager::appendSoftkeys(const QWidget &source, int level)
{
    Q_D(QSoftKeyManager);
    bool ret = false;
    QList<QAction*> actions = source.actions();
    for (int i = 0; i < actions.count(); ++i) {
        if (actions.at(i)->softKeyRole() != QAction::NoSoftKey) {
            d->requestedSoftKeyActions.insert(level, actions.at(i));
            ret = true;
        }
    }
    return ret;
}

QWidget *QSoftKeyManager::softkeySource(QWidget *previousSource, bool& recursiveMerging)
{
    Q_D(QSoftKeyManager);
    QWidget *source = NULL;
    if (!previousSource) {
        // Initial source is primarily focuswidget and secondarily activeWindow
        source = QApplication::focusWidget();
        if (!source)
            source = QApplication::activeWindow();
    } else {
        // Softkey merging is based on four criterias
        // 1. Implicit merging is used whenever focus widget does not specify any softkeys
        bool implicitMerging = d->requestedSoftKeyActions.isEmpty();
        // 2. Explicit merging with parent is used whenever WA_MergeSoftkeys widget attribute is set
        bool explicitMerging = previousSource->testAttribute(Qt::WA_MergeSoftkeys);
        // 3. Explicit merging with all parents
        recursiveMerging |= previousSource->testAttribute(Qt::WA_MergeSoftkeysRecursively);
        // 4. Implicit and explicit merging always stops at window boundary
        bool merging = (implicitMerging || explicitMerging || recursiveMerging) && !previousSource->isWindow();

        source = merging ? previousSource->parentWidget() : NULL;
    }
    return source;
}

bool QSoftKeyManager::handleUpdateSoftKeys()
{
    Q_D(QSoftKeyManager);
    int level = 0;
    d->requestedSoftKeyActions.clear();
    bool recursiveMerging = false;
    QWidget *source = softkeySource(NULL, recursiveMerging);
    do {
        if (source) {
            bool added = appendSoftkeys(*source, level);
            source = softkeySource(source, recursiveMerging);
            level = added ? ++level : level;
        }
    } while (source);

    d->updateSoftKeys_sys();
    return true;
}

bool QSoftKeyManager::event(QEvent *e)
{
#ifndef QT_NO_ACTION
    if (e->type() == QEvent::UpdateSoftKeys)
        return handleUpdateSoftKeys();
#endif //QT_NO_ACTION
    return false;
}

#ifdef Q_WS_S60
bool QSoftKeyManager::handleCommand(int command)
{
    return static_cast<QSoftKeyManagerPrivateS60*>(QSoftKeyManager::instance()->d_func())->handleCommand(command);
}
#endif

QT_END_NAMESPACE
#endif //QT_NO_SOFTKEYMANAGER
