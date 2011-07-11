/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qgenericplugin_qpa.h"

#ifndef QT_NO_LIBRARY

QT_BEGIN_NAMESPACE

/*!
    \class QGenericPlugin
    \ingroup plugins
    \ingroup qpa
    \since 4.8

    \brief The QGenericPlugin class is an abstract base class for
    window-system related plugins in Qt QPA.

    Note that this class is only available in Qt QPA.

    A mouse plugin can be created by subclassing
    QGenericPlugin and reimplementing the pure virtual keys() and
    create() functions. By exporting the derived class using the
    Q_EXPORT_PLUGIN2() macro, The default implementation of the
    QGenericPluginFactory class will automatically detect the plugin and
    load the driver into the server application at run-time. See \l
    {How to Create Qt Plugins} for details.

    \sa QGenericPluginFactory
*/

/*!
    \fn QStringList QGenericPlugin::keys() const

    Implement this function to return the list of valid keys, i.e. the
    drivers supported by this plugin.

    \sa create()
*/

/*!
    Constructs a plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    Q_EXPORT_PLUGIN2() macro, so there is no need for calling it
    explicitly.
*/
QGenericPlugin::QGenericPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the plugin.

    Note that Qt destroys a plugin automatically when it is no longer
    used, so there is no need for calling the destructor explicitly.
*/
QGenericPlugin::~QGenericPlugin()
{
}

/*!
    \fn QObject* QGenericPlugin::create(const QString &key, const QString& specification)

    Implement this function to create a driver matching the type
    specified by the given \a key and \a specification parameters. Note that
    keys are case-insensitive.

    \sa keys()
*/

QT_END_NAMESPACE

#endif // QT_NO_LIBRARY
