/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include "baselineprotocol.h"
#include <QLibraryInfo>
#include <QImage>
#include <QBuffer>
#include <QHostInfo>
#include <QSysInfo>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QTime>

#ifndef QMAKESPEC
#define QMAKESPEC "Unknown"
#endif

#if defined(Q_OS_WIN)
#include <QtCore/qt_windows.h>
#endif
#if defined(Q_OS_UNIX)
#include <time.h>
#endif
void BaselineProtocol::sysSleep(int ms)
{
#if defined(Q_OS_WIN)
    Sleep(DWORD(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

PlatformInfo::PlatformInfo(bool useLocal)
    : QMap<QString, QString>()
{
    if (useLocal) {
        insert(PI_HostName, QHostInfo::localHostName());
        insert(PI_QtVersion, QLS(qVersion()));
        insert(PI_QMakeSpec, QString(QLS(QMAKESPEC)).remove(QRegExp(QLS("^.*mkspecs/"))));
        insert(PI_BuildKey, QLibraryInfo::buildKey());
#if defined(Q_OS_LINUX)
        insert(PI_OSName, QLS("Linux"));
        QProcess uname;
        uname.start(QLS("uname"), QStringList() << QLS("-r"));
        if (uname.waitForFinished(3000))
            insert(PI_OSVersion, QString::fromLocal8Bit(uname.readAllStandardOutput().constData()).simplified());
#elif defined(Q_OS_WINCE)
        insert(PI_OSName, QLS("WinCE"));
        insert(PI_OSVersion, QString::number(QSysInfo::windowsVersion()));
#elif defined(Q_OS_WIN)
        insert(PI_OSName, QLS("Windows"));
        insert(PI_OSVersion, QString::number(QSysInfo::windowsVersion()));
#elif defined(Q_OS_MAC)
        insert(PI_OSName, QLS("MacOS"));
        insert(PI_OSVersion, QString::number(qMacVersion()));
#elif defined(Q_OS_SYMBIAN)
        insert(PI_OSName, QLS("Symbian"));
        insert(PI_OSVersion, QString::number(QSysInfo::symbianVersion());
#else
        insert(PI_OSName, QLS("Other"));
#endif

        QProcess git;
        QString cmd;
        QStringList args;
#if defined(Q_OS_WIN)
        cmd = QLS("cmd.exe");
        args << QLS("/c") << QLS("git");
#else
        cmd = QLS("git");
#endif
        args << QLS("log") << QLS("--max-count=1") << QLS("--pretty=%H [%an] [%ad] %s");
        git.start(cmd, args);
        git.waitForFinished(3000);
        if (!git.exitCode())
            insert(PI_GitCommit, QString::fromLocal8Bit(git.readAllStandardOutput().constData()).simplified());
        else
            insert(PI_GitCommit, QLS("Unknown"));
    }

    QByteArray gb = qgetenv("PULSE_GIT_BRANCH");
    if (!gb.isEmpty())
        insert(PI_PulseGitBranch, QString::fromLatin1(gb));
    QByteArray tb = qgetenv("PULSE_TESTR_BRANCH");
    if (!tb.isEmpty())
        insert(PI_PulseTestrBranch, QString::fromLatin1(tb));
}


ImageItem &ImageItem::operator=(const ImageItem &other)
{
    scriptName = other.scriptName;
    scriptChecksum = other.scriptChecksum;
    status = other.status;
    renderFormat = other.renderFormat;
    engine = other.engine;
    image = other.image;
    imageChecksums = other.imageChecksums;
    return *this;
}

// Defined in lookup3.c:
void hashword2 (
const quint32 *k,         /* the key, an array of quint32 values */
size_t         length,    /* the length of the key, in quint32s */
quint32       *pc,        /* IN: seed OUT: primary hash value */
quint32       *pb);       /* IN: more seed OUT: secondary hash value */

quint64 ImageItem::computeChecksum(const QImage &image)
{
    QImage img(image);
    const int bpl = img.bytesPerLine();
    const int padBytes = bpl - (img.width() * img.depth() / 8);
    if (padBytes) {
        uchar *p = img.bits() + bpl - padBytes;
        const int h = img.height();
        for (int y = 0; y < h; ++y) {
            qMemSet(p, 0, padBytes);
            p += bpl;
        }
    }

    quint32 h1 = 0xfeedbacc;
    quint32 h2 = 0x21604894;
    hashword2((const quint32 *)img.constBits(), img.byteCount()/4, &h1, &h2);
    return (quint64(h1) << 32) | h2;
}

QString ImageItem::engineAsString() const
{
    switch (engine) {
    case Raster:
        return QLS("Raster");
        break;
    case OpenGL:
        return QLS("OpenGL");
        break;
    default:
        break;
    }
    return QLS("Unknown");
}

QString ImageItem::formatAsString() const
{
    static const int numFormats = 16;
    static const char *formatNames[numFormats] = {
        "Invalid",
        "Mono",
        "MonoLSB",
        "Indexed8",
        "RGB32",
        "ARGB32",
        "ARGB32-Premult",
        "RGB16",
        "ARGB8565-Premult",
        "RGB666",
        "ARGB6666-Premult",
        "RGB555",
        "ARGB8555-Premult",
        "RGB888",
        "RGB444",
        "ARGB4444-Premult"
    };
    if (renderFormat < 0 || renderFormat >= numFormats)
        return QLS("UnknownFormat");
    return QLS(formatNames[renderFormat]);
}

void ImageItem::writeImageToStream(QDataStream &out) const
{
    if (image.isNull() || image.format() == QImage::Format_Invalid) {
        out << quint8(0);
        return;
    }
    out << quint8('Q') << quint8(image.format());
    out << quint8(QSysInfo::ByteOrder) << quint8(0);       // pad to multiple of 4 bytes
    out << quint32(image.width()) << quint32(image.height()) << quint32(image.bytesPerLine());
    out << qCompress((const uchar *)image.constBits(), image.byteCount());
    //# can be followed by colormap for formats that use it
}

void ImageItem::readImageFromStream(QDataStream &in)
{
    quint8 hdr, fmt, endian, pad;
    quint32 width, height, bpl;
    QByteArray data;

    in >> hdr;
    if (hdr != 'Q') {
        image = QImage();
        return;
    }
    in >> fmt >> endian >> pad;
    if (!fmt || fmt >= QImage::NImageFormats) {
        image = QImage();
        return;
    }
    if (endian != QSysInfo::ByteOrder) {
        qWarning("ImageItem cannot read streamed image with different endianness");
        image = QImage();
        return;
    }
    in >> width >> height >> bpl;
    in >> data;
    data = qUncompress(data);
    QImage res((const uchar *)data.constData(), width, height, bpl, QImage::Format(fmt));
    image = res.copy();  //# yuck, seems there is currently no way to avoid data copy
}

QDataStream & operator<< (QDataStream &stream, const ImageItem &ii)
{
    stream << ii.scriptName << ii.scriptChecksum << quint8(ii.status) << quint8(ii.renderFormat)
           << quint8(ii.engine) << ii.imageChecksums;
    ii.writeImageToStream(stream);
    return stream;
}

QDataStream & operator>> (QDataStream &stream, ImageItem &ii)
{
    quint8 encFormat, encStatus, encEngine;
    stream >> ii.scriptName >> ii.scriptChecksum >> encStatus >> encFormat
           >> encEngine >> ii.imageChecksums;
    ii.renderFormat = QImage::Format(encFormat);
    ii.status = ImageItem::ItemStatus(encStatus);
    ii.engine = ImageItem::GraphicsEngine(encEngine);
    ii.readImageFromStream(stream);
    return stream;
}

BaselineProtocol::~BaselineProtocol()
{
    socket.close();
    if (socket.state() != QTcpSocket::UnconnectedState)
        socket.waitForDisconnected(Timeout);
}


bool BaselineProtocol::connect(bool *dryrun)
{
    errMsg.clear();
    QByteArray serverName(qgetenv("QT_LANCELOT_SERVER"));
    if (serverName.isNull())
        serverName = "lancelot.test.qt.nokia.com";

    socket.connectToHost(serverName, ServerPort);
    if (!socket.waitForConnected(Timeout)) {
        sysSleep(Timeout);  // Wait a bit and try again, the server might just be restarting
        if (!socket.waitForConnected(Timeout)) {
            errMsg += QLS("TCP connectToHost failed. Host:") + serverName + QLS(" port:") + QString::number(ServerPort);
            return false;
        }
    }

    PlatformInfo pi(true);
    QByteArray block;
    QDataStream ds(&block, QIODevice::ReadWrite);
    ds << pi;
    if (!sendBlock(AcceptPlatformInfo, block)) {
        errMsg += QLS("Failed to send data to server.");
        return false;
    }

    Command cmd = UnknownError;
    if (!receiveBlock(&cmd, &block)) {
        errMsg += QLS("Failed to get response from server.");
        return false;
    }

    if (cmd == Abort) {
        errMsg += QLS("Server aborted connection. Reason: ") + QString::fromLatin1(block);
        return false;
    }

    if (dryrun)
        *dryrun = (cmd == DoDryRun);

    if (cmd != Ack && cmd != DoDryRun) {
        errMsg += QLS("Unexpected response from server.");
        return false;
    }

    return true;
}


bool BaselineProtocol::acceptConnection(PlatformInfo *pi)
{
    errMsg.clear();

    QByteArray block;
    Command cmd = AcceptPlatformInfo;
    if (!receiveBlock(&cmd, &block) || cmd != AcceptPlatformInfo)
        return false;

    if (pi) {
        QDataStream ds(block);
        ds >> *pi;
        pi->insert(PI_HostAddress, socket.peerAddress().toString());
    }

    return true;
}


bool BaselineProtocol::requestBaselineChecksums(ImageItemList *itemList)
{
    errMsg.clear();
    if (!itemList)
        return false;
    QByteArray block;
    QDataStream ds(&block, QIODevice::ReadWrite);
    ds << *itemList;
    if (!sendBlock(RequestBaselineChecksums, block))
        return false;
    Command cmd;
    if (!receiveBlock(&cmd, &block))
        return false;
    ds.device()->seek(0);
    ds >> *itemList;
    return true;
}


bool BaselineProtocol::submitNewBaseline(const ImageItem &item, QByteArray *serverMsg)
{
    Command cmd;
    return (sendItem(AcceptNewBaseline, item) && receiveBlock(&cmd, serverMsg) && cmd == Ack);
}


bool BaselineProtocol::submitMismatch(const ImageItem &item, QByteArray *serverMsg)
{
    Command cmd;
    return (sendItem(AcceptMismatch, item) && receiveBlock(&cmd, serverMsg) && cmd == Ack);
}


bool BaselineProtocol::sendItem(Command cmd, const ImageItem &item)
{
    errMsg.clear();
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    QDataStream ds(&buf);
    ds << item;
    if (!sendBlock(cmd, buf.data())) {
        errMsg.prepend(QLS("Failed to submit image to server. "));
        return false;
    }
    return true;
}


bool BaselineProtocol::sendBlock(Command cmd, const QByteArray &block)
{
    QDataStream s(&socket);
    // TBD: set qds version as a constant
    s << quint16(ProtocolVersion) << quint16(cmd);
    s.writeBytes(block.constData(), block.size());
    return true;
}


bool BaselineProtocol::receiveBlock(Command *cmd, QByteArray *block)
{
    while (socket.bytesAvailable() < int(2*sizeof(quint16) + sizeof(quint32))) {
        if (!socket.waitForReadyRead(Timeout))
            return false;
    }
    QDataStream ds(&socket);
    quint16 rcvProtocolVersion, rcvCmd;
    ds >> rcvProtocolVersion >> rcvCmd;
    if (rcvProtocolVersion != ProtocolVersion) {
        errMsg = QLS("Baseline protocol version mismatch, received:") + QString::number(rcvProtocolVersion)
                + QLS(" expected:") + QString::number(ProtocolVersion);
        return false;
    }
    if (cmd)
        *cmd = Command(rcvCmd);

    QByteArray uMsg;
    quint32 remaining;
    ds >> remaining;
    uMsg.resize(remaining);
    int got = 0;
    char* uMsgBuf = uMsg.data();
    do {
        got = ds.readRawData(uMsgBuf, remaining);
        remaining -= got;
        uMsgBuf += got;
    } while (remaining && got >= 0 && socket.waitForReadyRead(Timeout));

    if (got < 0)
        return false;

    if (block)
        *block = uMsg;

    return true;
}


QString BaselineProtocol::errorMessage()
{
    QString ret = errMsg;
    if (socket.error() >= 0)
        ret += QLS(" Socket state: ") + socket.errorString();
    return ret;
}
