/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QDebug>
#include <qtest.h>
#include <QtTest/QtTest>
#include <QFile>
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <QSize>

#if defined(Q_OS_SYMBIAN)
# define SRCDIR ""
#endif

class tst_jpeg : public QObject
{
    Q_OBJECT
private slots:
    void jpegDecodingQtWebkitStyle();
};

void tst_jpeg::jpegDecodingQtWebkitStyle()
{
    // QtWebkit currently calls size() to get the image size for layouting purposes.
    // Then when it is in the viewport (we assume that here) it actually gets decoded.
    QFile inputJpeg(SRCDIR "n900.jpeg");
    QVERIFY(inputJpeg.exists());
    inputJpeg.open(QIODevice::ReadOnly);
    QByteArray imageData = inputJpeg.readAll();
    QBuffer buffer;
    buffer.setData(imageData);
    buffer.open(QBuffer::ReadOnly);
    QCOMPARE(buffer.size(), qint64(19016));


    QBENCHMARK{
        for (int i = 0; i < 50; i++) {
            QImageReader reader(&buffer, "jpeg");
            QSize size = reader.size();
            QVERIFY(!size.isNull());
            QByteArray format = reader.format();
            QVERIFY(!format.isEmpty());
            QImage img = reader.read();
            QVERIFY(!img.isNull());
            buffer.reset();
        }
    }
}

QTEST_MAIN(tst_jpeg)

#include "jpeg.moc"
