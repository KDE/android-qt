/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativeintegercache_p.h"

#include "qdeclarativeengine_p.h"
#include "qdeclarativemetatype_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeIntegerCache::QDeclarativeIntegerCache(QDeclarativeEngine *e)
: QDeclarativeCleanup(e), engine(e)
{
}

QDeclarativeIntegerCache::~QDeclarativeIntegerCache()
{
    clear();
}

void QDeclarativeIntegerCache::clear()
{
    qDeleteAll(stringCache);
    stringCache.clear();
    identifierCache.clear();
    engine = 0;
}

void QDeclarativeIntegerCache::add(const QString &id, int value)
{
    Q_ASSERT(!stringCache.contains(id));

    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(engine);

    // ### use contextClass
    Data *d = new Data(enginePriv->objectClass->createPersistentIdentifier(id), value);

    stringCache.insert(id, d);
    identifierCache.insert(d->identifier, d);
}

int QDeclarativeIntegerCache::value(const QString &id)
{
    Data *d = stringCache.value(id);
    return d?d->value:-1;
}

QT_END_NAMESPACE
