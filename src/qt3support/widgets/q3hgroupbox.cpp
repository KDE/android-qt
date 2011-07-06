/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3hgroupbox.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3HGroupBox

    \brief The Q3HGroupBox widget organizes widgets in a group with one
    horizontal row.

    \compat

    Q3HGroupBox is a convenience class that offers a thin layer on top
    of Q3GroupBox. Think of it as a Q3HBox that offers a frame with a
    title.

    \sa Q3VGroupBox
*/

/*!
    Constructs a horizontal group box with no title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/
Q3HGroupBox::Q3HGroupBox( QWidget *parent, const char *name )
    : Q3GroupBox( 1, Qt::Vertical /* sic! */, parent, name )
{
}

/*!
    Constructs a horizontal group box with the title \a title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/

Q3HGroupBox::Q3HGroupBox( const QString &title, QWidget *parent,
			    const char *name )
    : Q3GroupBox( 1, Qt::Vertical /* sic! */, title, parent, name )
{
}

/*!
    Destroys the horizontal group box, deleting its child widgets.
*/
Q3HGroupBox::~Q3HGroupBox()
{
}

QT_END_NAMESPACE
