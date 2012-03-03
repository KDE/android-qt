/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>

#include <private/qringbuffer_p.h>

class tst_QRingBuffer : public QObject
{
    Q_OBJECT

public:
    tst_QRingBuffer();
    virtual ~tst_QRingBuffer();
public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void readPointerAtPositionWriteRead();
    void readPointerAtPositionEmptyRead();
    void readPointerAtPositionWithHead();
    void readPointerAtPositionReadTooMuch();
    void sizeWhenEmpty();
    void sizeWhenReservedAndChopped();
    void sizeWhenReserved();
};

tst_QRingBuffer::tst_QRingBuffer()
{
}

tst_QRingBuffer::~tst_QRingBuffer()
{
}

void tst_QRingBuffer::initTestCase()
{
}

void tst_QRingBuffer::cleanupTestCase()
{
}

void tst_QRingBuffer::sizeWhenReserved()
{
    QRingBuffer ringBuffer;
    ringBuffer.reserve(5);

    QCOMPARE(ringBuffer.size(), 5);
}

void tst_QRingBuffer::sizeWhenReservedAndChopped()
{
    QRingBuffer ringBuffer;
    ringBuffer.reserve(31337);
    ringBuffer.chop(31337);

    QCOMPARE(ringBuffer.size(), 0);
}

void tst_QRingBuffer::sizeWhenEmpty()
{
    QRingBuffer ringBuffer;

    QCOMPARE(ringBuffer.size(), 0);
}

void tst_QRingBuffer::readPointerAtPositionReadTooMuch()
{
    QRingBuffer ringBuffer;

    qint64 length;
    const char *buf = ringBuffer.readPointerAtPosition(42, length);
    QVERIFY(buf == 0);
    QVERIFY(length == 0);
}

void tst_QRingBuffer::readPointerAtPositionWithHead()
{
    QRingBuffer ringBuffer;
    char *buf = ringBuffer.reserve(4);
    memcpy (buf, "0123", 4);
    ringBuffer.free(2);

    // ringBuffer should have stayed the same except
    // its head it had moved to position 2
    qint64 length;
    const char* buf2 = ringBuffer.readPointerAtPosition(0, length);

    QCOMPARE(length, qint64(2));
    QVERIFY(*buf2 == '2');
    QVERIFY(*(buf2+1) == '3');

    // advance 2 more, ringBuffer should be empty then
    ringBuffer.free(2);
    buf2 = ringBuffer.readPointerAtPosition(0, length);
    QCOMPARE(length, qint64(0));
    QVERIFY(buf2 == 0);
}

void tst_QRingBuffer::readPointerAtPositionEmptyRead()
{
    QRingBuffer ringBuffer;

    qint64 length;
    const char *buf = ringBuffer.readPointerAtPosition(0, length);
    QVERIFY(buf == 0);
    QVERIFY(length == 0);
}

void tst_QRingBuffer::readPointerAtPositionWriteRead()
{
    //create some data
    QBuffer inData;
    inData.open(QIODevice::ReadWrite);
    inData.putChar(0x42);
    inData.putChar(0x23);
    inData.write("Qt rocks!");
    for (int i = 0; i < 5000; i++)
        inData.write(QString("Number %1").arg(i).toUtf8());
    inData.reset();
    QVERIFY(inData.size() > 0);

    //put the inData in the QRingBuffer
    QRingBuffer ringBuffer;
    qint64 remaining = inData.size();
    while (remaining > 0) {
        // write in chunks of 50 bytes
        // this ensures there will be multiple QByteArrays inside the QRingBuffer
        // since QRingBuffer is then only using individual arrays of around 4000 bytes
        qint64 thisWrite = qMin(remaining, qint64(50));
        char *pos = ringBuffer.reserve(thisWrite);
        inData.read(pos, thisWrite);
        remaining -= thisWrite;
    }
    // was data put into it?
    QVERIFY(ringBuffer.size() > 0);
    QCOMPARE(qint64(ringBuffer.size()), inData.size());

    //read from the QRingBuffer in loop, put back into another QBuffer
    QBuffer outData;
    outData.open(QIODevice::ReadWrite);
    remaining = ringBuffer.size();
    while (remaining > 0) {
        qint64 thisRead;
        // always try to read as much as possible
        const char *buf = ringBuffer.readPointerAtPosition(ringBuffer.size() - remaining, thisRead);
        outData.write(buf, thisRead);
        remaining -= thisRead;
    }
    outData.reset();

    QVERIFY(outData.size() > 0);

    // was the data read from the QRingBuffer the same as the one written into it?
    QCOMPARE(outData.size(), inData.size());
    QVERIFY(outData.buffer().startsWith(inData.buffer()));
}


QTEST_APPLESS_MAIN(tst_QRingBuffer)
#include "tst_qringbuffer.moc"
