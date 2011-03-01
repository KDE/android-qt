/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef __MENU_H__
#define __MENU_H__

#include <QGraphicsWidget>
#include <QList>

class QGraphicsView;
class QGraphicsLinearLayout;
class Button;

class Menu : public QGraphicsWidget
{
    Q_OBJECT
public:
    Menu(QGraphicsView* parent);
    ~Menu();

public:
    Button* addMenuItem(const QString itemName, QObject* receiver, const char* member);
    inline bool menuVisible() { return m_isMenuVisible; }
    virtual void keyPressEvent(QKeyEvent *event);

public slots:
    void themeChange();

public slots:
    void menuShowHide();
	
private:
    void init();
    void menuShow();
    void menuHide();
	
private:
    Q_DISABLE_COPY(Menu)
    QGraphicsView* m_Parent;
    QGraphicsLinearLayout* m_Layout;
    QList<Button*>* m_ButtonContainer;
    bool m_isMenuVisible;
    int m_currentSelectedIndex;
};

#endif // __MENU_H__ 
