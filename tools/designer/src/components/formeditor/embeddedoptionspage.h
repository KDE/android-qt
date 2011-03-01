/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef EMBEDDEDOPTIONSPAGE_H
#define EMBEDDEDOPTIONSPAGE_H

#include <QtDesigner/private/abstractoptionspage_p.h>
#include <QtCore/QPointer>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;

namespace qdesigner_internal {

class EmbeddedOptionsControlPrivate;

/* EmbeddedOptions Control. Presents the user with a list of embedded
 * device profiles he can modify/add/delete. */
class EmbeddedOptionsControl : public QWidget {
    Q_DISABLE_COPY(EmbeddedOptionsControl)
    Q_OBJECT
public:
    explicit EmbeddedOptionsControl(QDesignerFormEditorInterface *core, QWidget *parent = 0);
    ~EmbeddedOptionsControl();

    bool isDirty() const;

public slots:
    void loadSettings();
    void saveSettings();

private slots:
    void slotAdd();
    void slotEdit();
    void slotDelete();
    void slotProfileIndexChanged(int);

private:
    EmbeddedOptionsControlPrivate *m_d;
};

// EmbeddedOptionsPage
class EmbeddedOptionsPage : public QDesignerOptionsPageInterface
{
    Q_DISABLE_COPY(EmbeddedOptionsPage)
public:
    explicit EmbeddedOptionsPage(QDesignerFormEditorInterface *core);

    QString name() const;
    QWidget *createPage(QWidget *parent);
    virtual void finish();
    virtual void apply();

private:
    QDesignerFormEditorInterface *m_core;
    QPointer<EmbeddedOptionsControl> m_embeddedOptionsControl;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // EMBEDDEDOPTIONSPAGE_H
