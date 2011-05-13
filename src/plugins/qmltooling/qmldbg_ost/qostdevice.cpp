/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qostdevice.h"
#include <e32base.h>

#include "usbostcomm.h"

class QOstDevicePrivate : public CActive
{
    QOstDevice* q_ptr;
    Q_DECLARE_PUBLIC(QOstDevice)

public:
    QOstDevicePrivate() : CActive(CActive::EPriorityStandard) {
        CActiveScheduler::Add(this);
    }
    ~QOstDevicePrivate() {
        Cancel();
    }

private:
    void RunL();
    void DoCancel();

private:
    RUsbOstComm ost;
    TBuf8<4096> readBuf;
    QByteArray dataBuf;
};

QOstDevice::QOstDevice(QObject *parent) :
    QIODevice(parent), d_ptr(new QOstDevicePrivate)
{
    d_ptr->q_ptr = this;
}

QOstDevice::~QOstDevice()
{
    close();
    delete d_ptr;
}

bool QOstDevice::open(int ostProtocolId)
{
    if (isOpen())
        return false;

    Q_D(QOstDevice);
    TInt err = d->ost.Connect();
    if (!err) err = d->ost.Open();
    const TVersion KRequiredVersion(1,1,0);
    TVersion version = d->ost.Version();
    if (version.iMajor < KRequiredVersion.iMajor ||
            (version.iMajor == KRequiredVersion.iMajor && version.iMinor < KRequiredVersion.iMinor)) {
        setErrorString("CODA version too old. At least version 4.0.18 (without TRK) is required.");
        return false;
    }

    if (!err) err = d->ost.RegisterProtocolID((TOstProtIds)ostProtocolId, EFalse);
    if (!err) {
        d->ost.ReadMessage(d->iStatus, d->readBuf);
        d->SetActive();
        return QIODevice::open(ReadWrite | Unbuffered);
    }
    return false;
}

void QOstDevicePrivate::RunL()
{
    Q_Q(QOstDevice);
    //qDebug("QOstDevice received %d bytes q=%x", readBuf.Size(), q);
    if (iStatus == KErrNone) {
        QByteArray data = QByteArray::fromRawData((const char*)readBuf.Ptr(), readBuf.Size());
        dataBuf.append(data);

        readBuf.Zero();
        ost.ReadMessage(iStatus, readBuf);
        SetActive();

        emit q->readyRead();
    } else {
        q->setErrorString(QString("Error %1 from RUsbOstComm::ReadMessage()").arg(iStatus.Int()));
    }
    //qDebug("-QOstDevicePrivate RunL");
}

void QOstDevicePrivate::DoCancel()
{
    ost.ReadCancel();
}

void QOstDevice::close()
{
    Q_D(QOstDevice);
    QIODevice::close();
    d->Cancel();
    // RDbgTrcComm::Close isn't safe to call when not open, sigh
    if (d->ost.Handle()) {
        d->ost.Close();
    }
}

qint64 QOstDevice::readData(char *data, qint64 maxSize)
{
    Q_D(QOstDevice);
    if (d->dataBuf.length() == 0 && !d->IsActive())
        return -1;
    qint64 available = qMin(maxSize, (qint64)d->dataBuf.length());
    memcpy(data, d->dataBuf.constData(), available);
    d->dataBuf.remove(0, available);
    return available;
}

static const TInt KMaxOstPacketLen = 4096;

qint64 QOstDevice::writeData(const char *data, qint64 maxSize)
{
    Q_D(QOstDevice);
    TPtrC8 ptr((const TUint8*)data, (TInt)maxSize);
    while (ptr.Length()) {
        TPtrC8 fragment = ptr.Left(qMin(ptr.Length(), KMaxOstPacketLen));
        //qDebug("QOstDevice writing %d bytes", fragment.Length());
        TRequestStatus stat;
        d->ost.WriteMessage(stat, fragment);
        User::WaitForRequest(stat);
        if (stat.Int() != KErrNone) {
            setErrorString(QString("Error %1 from RUsbOstComm::WriteMessage()").arg(stat.Int()));
            return -1;
        }
        ptr.Set(ptr.Mid(fragment.Length()));
    }
    emit bytesWritten(maxSize); //TODO does it matter this is emitted synchronously?
    //qDebug("QOstDevice wrote %d bytes", ptr.Size());
    return maxSize;
}

qint64 QOstDevice::bytesAvailable() const
{
    Q_D(const QOstDevice);
    return d->dataBuf.length();
}
