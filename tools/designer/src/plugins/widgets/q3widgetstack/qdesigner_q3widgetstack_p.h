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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_Q3WIDGETSTACK_P_H
#define QDESIGNER_Q3WIDGETSTACK_P_H

#include <Qt3Support/Q3WidgetStack>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerContainerExtension;
class QToolButton;
class QChildEvent;
class QResizeEvent;
class QShowEvent;
class QEvent;

class QDesignerQ3WidgetStack : public Q3WidgetStack
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex STORED false DESIGNABLE true)
    Q_PROPERTY(QString currentPageName READ currentPageName WRITE setCurrentPageName STORED false DESIGNABLE true)
public:
    QDesignerQ3WidgetStack(QWidget *parent = 0);
    int currentIndex();
    QString currentPageName();

public slots:
    void updateButtons();
    void setCurrentIndex(int index);
    void setCurrentPageName(const QString &pageName);

private slots:
    void prevPage();
    void nextPage();
    void slotCurrentChanged(int index);

signals:
    void currentChanged(int index);

protected:
    virtual void childEvent(QChildEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual bool event(QEvent *e);

private:
    void gotoPage(int page);
    QDesignerFormWindowInterface *formWindow();
    QDesignerContainerExtension *container();
    int count();
    QWidget *widget(int index);
    QToolButton *m_prev, *m_next;
};

QT_END_NAMESPACE

#endif // !QDESIGNER_Q3WIDGETSTACK_P_H
