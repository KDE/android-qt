/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef S60MEDIAPLAYERAUDIOENDPOINTSELECTOR_H
#define S60MEDIAPLAYERAUDIOENDPOINTSELECTOR_H

#include <QStringList>

#include <QtMediaServices/qmediacontrol.h>

QT_BEGIN_NAMESPACE

class S60MediaPlayerControl;
class S60MediaPlayerSession;

class S60MediaPlayerAudioEndpointSelector : public QMediaControl
{

Q_OBJECT

public:
    S60MediaPlayerAudioEndpointSelector(QObject *control, QObject *parent = 0);
    ~S60MediaPlayerAudioEndpointSelector();

    QList<QString> availableEndpoints() const ;
    QString endpointDescription(const QString& name) const;
    QString defaultEndpoint() const;
    QString activeEndpoint() const;

public Q_SLOTS:
    void setActiveEndpoint(const QString& name);

private:
    S60MediaPlayerControl* m_control;
    QString m_audioInput;
    QList<QString> *m_audioEndpointNames;
};

#define QAudioEndpointSelector_iid "com.nokia.Qt.QAudioEndpointSelector/1.0"

QT_END_NAMESPACE

#endif // S60MEDIAPLAYERAUDIOENDPOINTSELECTOR_H
