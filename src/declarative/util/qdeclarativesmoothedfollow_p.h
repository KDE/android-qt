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

#ifndef QDECLARATIVESMOOTHEDFOLLOW_H
#define QDECLARATIVESMOOTHEDFOLLOW_H

#include <qdeclarative.h>
#include <qdeclarativepropertyvaluesource.h>

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeProperty;
class QDeclarativeSmoothedFollowPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeSmoothedFollow : public QObject,
                                                        public QDeclarativePropertyValueSource
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeSmoothedFollow)
    Q_INTERFACES(QDeclarativePropertyValueSource)
    Q_ENUMS(ReversingMode)

    Q_PROPERTY(qreal to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(qreal velocity READ velocity WRITE setVelocity NOTIFY velocityChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(ReversingMode reversingMode READ reversingMode WRITE setReversingMode NOTIFY reversingModeChanged)
    Q_PROPERTY(qreal maximumEasingTime READ maximumEasingTime WRITE setMaximumEasingTime NOTIFY maximumEasingTimeChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    enum ReversingMode { Eased, Immediate, Sync };

    QDeclarativeSmoothedFollow(QObject *parent = 0);
    ~QDeclarativeSmoothedFollow();

    qreal to() const;
    void setTo(qreal);

    ReversingMode reversingMode() const;
    void setReversingMode(ReversingMode);

    int duration() const;
    void setDuration(int);

    qreal velocity() const;
    void setVelocity(qreal);

    int maximumEasingTime() const;
    void setMaximumEasingTime(int);

    bool enabled() const;
    void setEnabled(bool);

    virtual void setTarget(const QDeclarativeProperty &);

Q_SIGNALS:
    void velocityChanged();
    void durationChanged();
    void reversingModeChanged();
    void maximumEasingTimeChanged();
    void enabledChanged();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeSmoothedFollow);

QT_END_HEADER

#endif // QDECLARATIVESMOOTHEDFOLLOW_H
