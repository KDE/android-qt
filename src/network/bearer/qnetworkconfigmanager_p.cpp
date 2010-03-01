/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qnetworkconfigmanager_p.h"
#include "qbearerplugin_p.h"

#include <QtCore/private/qfactoryloader_p.h>

#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstringlist.h>
#include <QtCore/private/qcoreapplication_p.h>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
                          (QBearerEngineFactoryInterface_iid, QLatin1String("/bearer")))

QNetworkConfigurationManagerPrivate::QNetworkConfigurationManagerPrivate()
:   capFlags(0), mutex(QMutex::Recursive), firstUpdate(true)
{
    updateConfigurations();

    moveToThread(QCoreApplicationPrivate::mainThread());
    foreach (QBearerEngine *engine, sessionEngines)
        engine->moveToThread(QCoreApplicationPrivate::mainThread());
}

QNetworkConfigurationManagerPrivate::~QNetworkConfigurationManagerPrivate()
{
    QMutexLocker locker(&mutex);

    qDeleteAll(sessionEngines);
}

void QNetworkConfigurationManagerPrivate::configurationAdded(QNetworkConfigurationPrivatePointer ptr)
{
    QMutexLocker locker(&mutex);

    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationAdded(item);
    }

    if (ptr->state == QNetworkConfiguration::Active) {
        onlineConfigurations.insert(ptr);
        if (!firstUpdate && onlineConfigurations.count() == 1)
            emit onlineStateChanged(true);
    }
}

void QNetworkConfigurationManagerPrivate::configurationRemoved(QNetworkConfigurationPrivatePointer ptr)
{
    QMutexLocker locker(&mutex);

    ptr->isValid = false;

    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationRemoved(item);
    }

    onlineConfigurations.remove(ptr);
    if (!firstUpdate && onlineConfigurations.isEmpty())
        emit onlineStateChanged(false);
}

void QNetworkConfigurationManagerPrivate::configurationChanged(QNetworkConfigurationPrivatePointer ptr)
{
    QMutexLocker locker(&mutex);

    if (!firstUpdate) {
        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationChanged(item);
    }

    bool previous = !onlineConfigurations.isEmpty();

    if (ptr->state == QNetworkConfiguration::Active)
        onlineConfigurations.insert(ptr);
    else
        onlineConfigurations.remove(ptr);

    bool online = !onlineConfigurations.isEmpty();

    if (!firstUpdate && online != previous)
        emit onlineStateChanged(online);
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    QMutexLocker locker(&mutex);

    if (firstUpdate) {
        updating = false;

        QFactoryLoader *l = loader();

        QBearerEngine *generic = 0;

        foreach (const QString &key, l->keys()) {
            QBearerEnginePlugin *plugin = qobject_cast<QBearerEnginePlugin *>(l->instance(key));
            if (plugin) {
                QBearerEngine *engine = plugin->create(key);
                if (!engine)
                    continue;

                if (key == QLatin1String("generic"))
                    generic = engine;
                else
                    sessionEngines.append(engine);

                connect(engine, SIGNAL(updateCompleted()),
                        this, SLOT(updateConfigurations()));
                connect(engine, SIGNAL(configurationAdded(QNetworkConfigurationPrivatePointer)),
                        this, SLOT(configurationAdded(QNetworkConfigurationPrivatePointer)));
                connect(engine, SIGNAL(configurationRemoved(QNetworkConfigurationPrivatePointer)),
                        this, SLOT(configurationRemoved(QNetworkConfigurationPrivatePointer)));
                connect(engine, SIGNAL(configurationChanged(QNetworkConfigurationPrivatePointer)),
                        this, SLOT(configurationChanged(QNetworkConfigurationPrivatePointer)));

                capFlags |= engine->capabilities();
            }
        }

        if (generic)
            sessionEngines.append(generic);
    }

    QBearerEngine *engine = qobject_cast<QBearerEngine *>(sender());
    if (!updatingEngines.isEmpty() && engine) {
        int index = sessionEngines.indexOf(engine);
        if (index >= 0)
            updatingEngines.remove(index);
    }

    if (updating && updatingEngines.isEmpty()) {
        updating = false;
        emit configurationUpdateComplete();
    }

    if (firstUpdate)
        firstUpdate = false;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    QMutexLocker locker(&mutex);

    if (sessionEngines.isEmpty()) {
        emit configurationUpdateComplete();
        return;
    }

    updating = true;

    for (int i = 0; i < sessionEngines.count(); ++i) {
        updatingEngines.insert(i);
        sessionEngines.at(i)->requestUpdate();
    }
}

QList<QBearerEngine *> QNetworkConfigurationManagerPrivate::engines()
{
    QMutexLocker locker(&mutex);

    return sessionEngines;
}

QT_END_NAMESPACE
