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

#include "qbaselinetest.h"
#include "baselineprotocol.h"

namespace QBaselineTest {

BaselineProtocol proto;
bool connected = false;
bool triedConnecting = false;

QByteArray curFunction;
ImageItemList itemList;
bool gotBaselines;


bool connect(QByteArray *msg, bool *error)
{
    if (!triedConnecting) {
        triedConnecting = true;
        if (!proto.connect(QTest::testObject()->metaObject()->className())) {
            *msg += "Failed to connect to baseline server: " + proto.errorMessage().toLatin1();
            *error = true;
            return false;
        }
        connected = true;
    }
    if (!connected) {
        *msg = "Not connected to baseline server.";
        *error = true;
        return false;
    }
    return true;
}


bool compareItem(const ImageItem &baseline, const QImage &img, QByteArray *msg, bool *error)
{
    ImageItem item = baseline;
    item.image = img;
    item.imageChecksums.clear();
    item.imageChecksums.prepend(ImageItem::computeChecksum(img));
    QByteArray srvMsg;
    switch (baseline.status) {
    case ImageItem::Ok:
        break;
    case ImageItem::IgnoreItem :
        qDebug() << msg->constData() << "Ignored, blacklisted on server.";
        return true;
        break;
    case ImageItem::BaselineNotFound:
        if (proto.submitNewBaseline(item, &srvMsg))
            qDebug() << msg->constData() << "Baseline not found on server. New baseline uploaded.";
        else
            qDebug() << msg->constData() << "Baseline not found on server. Uploading of new baseline failed:" << srvMsg;
        return true;
        break;
    default:
        qWarning() << "Unexpected reply from baseline server.";
        return true;
        break;
    }
    *error = false;
    // The actual comparison of the given image with the baseline:
    if (baseline.imageChecksums.contains(item.imageChecksums.at(0)))
        return true;
    proto.submitMismatch(item, &srvMsg);
    *msg += "Mismatch. See report:\n   " + srvMsg;
    return false;
}

bool checkImage(const QImage &img, const char *name, quint16 checksum, QByteArray *msg, bool *error)
{
    if (!connected && !connect(msg, error))
        return true;

    QByteArray itemName;
    bool hasName = qstrlen(name);
    const char *tag = QTest::currentDataTag();
    if (qstrlen(tag)) {
        itemName = tag;
        if (hasName)
            itemName.append('_').append(name);
    } else {
        itemName = hasName ? name : "default_name";
    }

    *msg = "Baseline check of image '" + itemName + "': ";


    ImageItem item;
    item.itemName = QString::fromLatin1(itemName);
    item.itemChecksum = checksum;
    item.testFunction = QString::fromLatin1(QTest::currentTestFunction());
    ImageItemList list;
    list.append(item);
    if (!proto.requestBaselineChecksums(QLatin1String(QTest::currentTestFunction()), &list) || list.isEmpty()) {
        *msg = "Communication with baseline server failed: " + proto.errorMessage().toLatin1();
        *error = true;
        return true;
    }

    return compareItem(list.at(0), img, msg, error);
}


QTestData &newRow(const char *dataTag, quint16 checksum)
{
    if (QTest::currentTestFunction() != curFunction) {
        curFunction = QTest::currentTestFunction();
        itemList.clear();
        gotBaselines = false;
    }
    ImageItem item;
    item.itemName = QString::fromLatin1(dataTag);
    item.itemChecksum = checksum;
    item.testFunction = QString::fromLatin1(QTest::currentTestFunction());
    itemList.append(item);

    return QTest::newRow(dataTag);
}


bool testImage(const QImage& img, QByteArray *msg, bool *error)
{
    if (!connected && !connect(msg, error))
        return true;

    if (QTest::currentTestFunction() != curFunction || itemList.isEmpty()) {
        qWarning() << "Usage error: QBASELINE_TEST used without corresponding QBaselineTest::newRow()";
        return true;
    }

    if (!gotBaselines) {
        if (!proto.requestBaselineChecksums(QString::fromLatin1(QTest::currentTestFunction()), &itemList) || itemList.isEmpty()) {
            *msg = "Communication with baseline server failed: " + proto.errorMessage().toLatin1();
            *error = true;
            return true;
        }
        gotBaselines = true;
    }

    QString curTag = QString::fromLatin1(QTest::currentDataTag());
    ImageItemList::const_iterator it = itemList.constBegin();
    while (it != itemList.constEnd() && it->itemName != curTag)
        ++it;
    if (it == itemList.constEnd()) {
        qWarning() << "Usage error: QBASELINE_TEST used without corresponding QBaselineTest::newRow() for row" << curTag;
        return true;
    }
    return compareItem(*it, img, msg, error);
}

}
