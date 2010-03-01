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

#ifndef SYMBIANAUDIOINPUT_H
#define SYMBIANAUDIOINPUT_H

#include <QtMultimedia/qaudioengine.h>
#include <QTime>
#include <QTimer>
#include <sounddevice.h>
#include "symbianaudio.h"

QT_BEGIN_NAMESPACE

class SymbianAudioInput;

class SymbianAudioInputPrivate : public QIODevice
{
    friend class SymbianAudioInput;
    Q_OBJECT
public:
    SymbianAudioInputPrivate(SymbianAudioInput *audio);
    ~SymbianAudioInputPrivate();

    qint64 readData(char *data, qint64 len);
    qint64 writeData(const char *data, qint64 len);

    void dataReady();

private:
    SymbianAudioInput *const m_audioDevice;
};

class SymbianAudioInput
    :   public QAbstractAudioInput
    ,   public MDevSoundObserver
{
    friend class SymbianAudioInputPrivate;
    Q_OBJECT
public:
    SymbianAudioInput(const QByteArray &device,
                      const QAudioFormat &audioFormat);
    ~SymbianAudioInput();

    // QAbstractAudioInput
    QIODevice* start(QIODevice *device = 0);
    void stop();
    void reset();
    void suspend();
    void resume();
    int bytesReady() const;
    int periodSize() const;
    void setBufferSize(int value);
    int bufferSize() const;
    void setNotifyInterval(int milliSeconds);
    int notifyInterval() const;
    qint64 processedUSecs() const;
    qint64 elapsedUSecs() const;
    QAudio::Error error() const;
    QAudio::State state() const;
    QAudioFormat format() const;

    // MDevSoundObserver
    void InitializeComplete(TInt aError);
    void ToneFinished(TInt aError);
    void BufferToBeFilled(CMMFBuffer *aBuffer);
    void PlayError(TInt aError);
    void BufferToBeEmptied(CMMFBuffer *aBuffer);
    void RecordError(TInt aError);
    void ConvertError(TInt aError);
    void DeviceMessage(TUid aMessageType, const TDesC8 &aMsg);

private slots:
    void pullData();

private:
   void open();
   void startRecording();
   void startDevSoundL();
   void startDataTransfer();
   CMMFDataBuffer* currentBuffer() const;
   void pushData();
   qint64 read(char *data, qint64 len);
   void bufferEmptied();
   Q_INVOKABLE void close();

   qint64 getSamplesRecorded() const;

   void setError(QAudio::Error error);
   void setState(SymbianAudio::State state);

   QAudio::State initializingState() const;

private:
    const QByteArray m_device;
    const QAudioFormat m_format;

    int m_clientBufferSize;
    int m_notifyInterval;
    QScopedPointer<QTimer> m_notifyTimer;
    QTime m_elapsed;
    QAudio::Error m_error;

    SymbianAudio::State m_internalState;
    QAudio::State m_externalState;

    bool m_pullMode;
    QIODevice *m_sink;

    QScopedPointer<QTimer> m_pullTimer;

    QScopedPointer<CMMFDevSound> m_devSound;
    TUint32 m_nativeFourCC;
    TMMFCapabilities m_nativeFormat;

    // Latest buffer provided by DevSound, to be empied of data.
    CMMFDataBuffer *m_devSoundBuffer;

    int m_devSoundBufferSize;

    // Total amount of data in buffers provided by DevSound
    int m_totalBytesReady;

    // Queue of buffers returned after call to CMMFDevSound::Pause().
    QList<CMMFDataBuffer *> m_devSoundBufferQ;

    // Current read position within m_devSoundBuffer
    qint64 m_devSoundBufferPos;

    // Samples recorded up to the last call to suspend().  It is necessary
    // to cache this because suspend() is implemented using
    // CMMFDevSound::Stop(), which resets DevSound's SamplesRecorded() counter.
    quint32 m_totalSamplesRecorded;

};

QT_END_NAMESPACE

#endif
