/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Assistant module of the Qt Toolkit.
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

#ifndef OPENPAGESWIDGET_H
#define OPENPAGESWIDGET_H

#include <QtGui/QStyledItemDelegate>
#include <QtGui/QTreeView>

QT_BEGIN_NAMESPACE

class OpenPagesModel;

class OpenPagesDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit OpenPagesDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    mutable QModelIndex pressedIndex;
};

class OpenPagesWidget : public QTreeView
{
    Q_OBJECT
public:
    OpenPagesWidget(OpenPagesModel *model);
    ~OpenPagesWidget();

    void selectCurrentPage();
    void allowContextMenu(bool ok);

signals:
    void setCurrentPage(const QModelIndex &index);
    void closePage(const QModelIndex &index);
    void closePagesExcept(const QModelIndex &index);

private slots:
    void contextMenuRequested(QPoint pos);
    void handlePressed(const QModelIndex &index);
    void handleClicked(const QModelIndex &index);

private:
    bool eventFilter(QObject *obj, QEvent *event);

    bool m_allowContextMenu;
    OpenPagesDelegate *m_delegate;
};

QT_END_NAMESPACE

#endif // OPENPAGESWIDGET_H
