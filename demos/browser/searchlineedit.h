/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "urllineedit.h"

#include <QtGui/QLineEdit>
#include <QtGui/QAbstractButton>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class SearchButton;

/*
    Clear button on the right hand side of the search widget.
    Hidden by default
    "A circle with an X in it"
 */
class ClearButton : public QAbstractButton
{
    Q_OBJECT

public:
    ClearButton(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);

public slots:
    void textChanged(const QString &text);
};


class SearchLineEdit : public ExLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString inactiveText READ inactiveText WRITE setInactiveText)

signals:
    void textChanged(const QString &text);

public:
    SearchLineEdit(QWidget *parent = 0);

    QString inactiveText() const;
    void setInactiveText(const QString &text);

    QMenu *menu() const;
    void setMenu(QMenu *menu);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void updateGeometries();

    SearchButton *m_searchButton;
    QString m_inactiveText;
};

#endif // SEARCHLINEEDIT_H

