/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSCriptTools module of the Qt Toolkit.
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

#include "qscriptdebuggerstackwidget_p.h"
#include "qscriptdebuggerstackwidgetinterface_p_p.h"

#include <QtCore/qdebug.h>
#include <QtGui/qheaderview.h>
#include <QtGui/qtreeview.h>
#include <QtGui/qboxlayout.h>

QT_BEGIN_NAMESPACE

class QScriptDebuggerStackWidgetPrivate
    : public QScriptDebuggerStackWidgetInterfacePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerStackWidget)
public:
    QScriptDebuggerStackWidgetPrivate();
    ~QScriptDebuggerStackWidgetPrivate();

    // private slots
    void _q_onCurrentChanged(const QModelIndex &index);

    QTreeView *view;
};

QScriptDebuggerStackWidgetPrivate::QScriptDebuggerStackWidgetPrivate()
{
}

QScriptDebuggerStackWidgetPrivate::~QScriptDebuggerStackWidgetPrivate()
{
}

void QScriptDebuggerStackWidgetPrivate::_q_onCurrentChanged(const QModelIndex &index)
{
    Q_Q(QScriptDebuggerStackWidget);
    emit q->currentFrameChanged(index.row());
}

QScriptDebuggerStackWidget::QScriptDebuggerStackWidget(QWidget *parent)
    : QScriptDebuggerStackWidgetInterface(*new QScriptDebuggerStackWidgetPrivate, parent, 0)
{
    Q_D(QScriptDebuggerStackWidget);
    d->view = new QTreeView();
    d->view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->view->setAlternatingRowColors(true);
    d->view->setRootIsDecorated(false);
    d->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->view->header()->setDefaultAlignment(Qt::AlignLeft);
//    d->view->header()->setResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->addWidget(d->view);
}

QScriptDebuggerStackWidget::~QScriptDebuggerStackWidget()
{
}

/*!
  \reimp
*/
QAbstractItemModel *QScriptDebuggerStackWidget::stackModel() const
{
    Q_D(const QScriptDebuggerStackWidget);
    return d->view->model();
}

/*!
  \reimp
*/
void QScriptDebuggerStackWidget::setStackModel(QAbstractItemModel *model)
{
    Q_D(QScriptDebuggerStackWidget);
    d->view->setModel(model);
    QObject::connect(d->view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                     this, SLOT(_q_onCurrentChanged(QModelIndex)));
    d->view->header()->resizeSection(0, 50);
}

/*!
  \reimp
*/
int QScriptDebuggerStackWidget::currentFrameIndex() const
{
    Q_D(const QScriptDebuggerStackWidget);
    return d->view->currentIndex().row();
}

/*!
  \reimp
*/
void QScriptDebuggerStackWidget::setCurrentFrameIndex(int frameIndex)
{
    Q_D(QScriptDebuggerStackWidget);
    d->view->setCurrentIndex(d->view->model()->index(frameIndex, 0));
}

QT_END_NAMESPACE

#include "moc_qscriptdebuggerstackwidget_p.cpp"
