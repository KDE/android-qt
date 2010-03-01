/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#ifndef SYMBIANAUDIODEVICEINFO_H
#define SYMBIANAUDIODEVICEINFO_H

#include <QtMultimedia/qaudioengine.h>
#include <sounddevice.h>

QT_BEGIN_NAMESPACE

class SymbianAudioDeviceInfo
    :   public QAbstractAudioDeviceInfo
{
    Q_OBJECT

public:
    SymbianAudioDeviceInfo(QByteArray device, QAudio::Mode mode);
    ~SymbianAudioDeviceInfo();

    // QAbstractAudioDeviceInfo
    QAudioFormat preferredFormat() const;
    bool isFormatSupported(const QAudioFormat &format) const;
    QAudioFormat nearestFormat(const QAudioFormat &format) const;
    QString deviceName() const;
    QStringList codecList();
    QList<int> frequencyList();
    QList<int> channelsList();
    QList<int> sampleSizeList();
    QList<QAudioFormat::Endian> byteOrderList();
    QList<QAudioFormat::SampleType> sampleTypeList();
    QList<QByteArray> deviceList(QAudio::Mode);

private:
    void getSupportedFormats() const;

private:
    QScopedPointer<CMMFDevSound> m_devsound;

    QString m_deviceName;
    QAudio::Mode m_mode;

    // Mutable to allow lazy initialization when called from const-qualified
    // public functions (isFormatSupported, nearestFormat)
    mutable bool m_updated;
    mutable QStringList m_codecs;
    mutable QList<int> m_frequencies;
    mutable QList<int> m_channels;
    mutable QList<int> m_sampleSizes;
    mutable QList<QAudioFormat::Endian> m_byteOrders;
    mutable QList<QAudioFormat::SampleType> m_sampleTypes;
};

QT_END_NAMESPACE

#endif
