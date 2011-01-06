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

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS

#include "baselineserver.h"
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QHostInfo>
#include <QTextStream>
#include <QProcess>
#include <QDirIterator>

QString BaselineServer::storage;


BaselineServer::BaselineServer(QObject *parent)
    : QTcpServer(parent)
{
    QFileInfo me(QCoreApplication::applicationFilePath());
    meLastMod = me.lastModified();
    heartbeatTimer = new QTimer(this);
    connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
    heartbeatTimer->start(HEARTBEAT*1000);
}

QString BaselineServer::storagePath()
{
    if (storage.isEmpty()) {
        storage = QLS(qgetenv("QT_LANCELOT_DIR"));
        if (storage.isEmpty())
            storage =  QLS("/var/www");
    }
    return storage;
}

QString BaselineServer::baseUrl()
{
    return QLS("http://")
            + QHostInfo::localHostName().toLatin1() + '.'
            + QHostInfo::localDomainName().toLatin1() + '/';
}

void BaselineServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "Server: New connection!";
    BaselineThread *thread = new BaselineThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void BaselineServer::heartbeat()
{
    // The idea is to exit to be restarted when modified, as soon as not actually serving
    QFileInfo me(QCoreApplication::applicationFilePath());
    if (me.lastModified() == meLastMod)
        return;

    //# (could close() here to avoid accepting new connections, to avoid livelock)
    //# also, could check for a timeout to force exit, to avoid hung threads blocking
    bool isServing = false;
    foreach(BaselineThread *thread, findChildren<BaselineThread *>()) {
        if (thread->isRunning()) {
            isServing = true;
            break;
        }
    }

    if (!isServing)
        QCoreApplication::exit();
}

BaselineThread::BaselineThread(int socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{
}

void BaselineThread::run()
{
    BaselineHandler handler(socketDescriptor);
    exec();
}


BaselineHandler::BaselineHandler(int socketDescriptor)
    : QObject(), connectionEstablished(false)
{
    runId = QDateTime::currentDateTime().toString(QLS("MMMdd-hhmmss"));

    if (socketDescriptor == -1)
        return;

    connect(&proto.socket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
    connect(&proto.socket, SIGNAL(disconnected()), this, SLOT(receiveDisconnect()));
    proto.socket.setSocketDescriptor(socketDescriptor);
}

const char *BaselineHandler::logtime()
{
    return 0;
    //return QTime::currentTime().toString(QLS("mm:ss.zzz"));
}

void BaselineHandler::receiveRequest()
{
    if (!connectionEstablished) {
        if (!proto.acceptConnection(&plat)) {
            qWarning() << runId << logtime() << "Accepting new connection from" << proto.socket.peerAddress().toString() << "failed." << proto.errorMessage();
            proto.socket.disconnectFromHost();
            return;
        }
        QString logMsg;
        foreach (QString key, plat.keys()) {
            if (key != PI_HostName && key != PI_HostAddress)
                logMsg += key + QLS(": '") + plat.value(key) + QLS("', ");
        }
        qDebug() << runId << logtime() << "Connection established with" << plat.value(PI_HostName)
                 << "[" << qPrintable(plat.value(PI_HostAddress)) << "]" << logMsg;

        // Filter on branch
        QString branch = plat.value(PI_PulseGitBranch);
        if (branch.isEmpty()) {
            // Not run by Pulse, i.e. ad hoc run: Ok.
        }
        else if (branch != QLS("master-integration") || !plat.value(PI_GitCommit).contains(QLS("Merge branch 'master' of scm.dev.nokia.troll.no:qt/oslo-staging-2 into master-integration"))) {
            qDebug() << runId << logtime() << "Did not pass branch/staging repo filter, disconnecting.";
            proto.sendBlock(BaselineProtocol::Abort, QByteArray("This branch/staging repo is not assigned to be tested."));
            proto.socket.disconnectFromHost();
            return;
        }

        proto.sendBlock(BaselineProtocol::Ack, QByteArray());
        connectionEstablished = true;
        return;
    }

    QByteArray block;
    BaselineProtocol::Command cmd;
    if (!proto.receiveBlock(&cmd, &block)) {
        qWarning() << runId << logtime() << "Command reception failed. "<< proto.errorMessage();
        QThread::currentThread()->exit(1);
        return;
    }

    switch(cmd) {
    case BaselineProtocol::RequestBaselineChecksums:
        provideBaselineChecksums(block);
        break;
    case BaselineProtocol::AcceptNewBaseline:
        storeImage(block, true);
        break;
    case BaselineProtocol::AcceptMismatch:
        storeImage(block, false);
        break;
    default:
        qWarning() << runId << logtime() << "Unknown command received. " << proto.errorMessage();
        proto.sendBlock(BaselineProtocol::UnknownError, QByteArray());
    }
}


void BaselineHandler::provideBaselineChecksums(const QByteArray &itemListBlock)
{
    ImageItemList itemList;
    QDataStream ds(itemListBlock);
    ds >> itemList;
    qDebug() << runId << logtime() << "Received request for checksums for" << itemList.count() << "items, engine"
             << itemList.at(0).engineAsString() << "pixel format" << itemList.at(0).formatAsString();

    for (ImageItemList::iterator i = itemList.begin(); i != itemList.end(); ++i) {
        i->imageChecksums.clear();
        QString prefix = pathForItem(*i, true);
        QFile file(prefix + QLS("metadata"));
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream checkSums(&file);
            checkSums >> i->imageChecksums;
            file.close();
            i->status = ImageItem::Ok;
        }
        if (!i->imageChecksums.count())
            i->status = ImageItem::BaselineNotFound;
    }

    // Find and mark blacklisted items
    QString context = pathForItem(itemList.at(0), true, false).section(QLC('/'), 0, -2);
    if (itemList.count() > 0) {
        QFile file(BaselineServer::storagePath() + QLC('/') + context + QLS("/BLACKLIST"));
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            do {
                QString scriptName = in.readLine();
                if (!scriptName.isNull()) {
                    for (ImageItemList::iterator i = itemList.begin(); i != itemList.end(); ++i) {
                        if (i->scriptName == scriptName)
                            i->status = ImageItem::IgnoreItem;
                    }
                }
            } while (!in.atEnd());
        }
    }

    QByteArray block;
    QDataStream ods(&block, QIODevice::WriteOnly);
    ods << itemList;
    proto.sendBlock(BaselineProtocol::Ack, block);
    report.start(BaselineServer::storagePath(), runId, plat, context, itemList);
}


void BaselineHandler::storeImage(const QByteArray &itemBlock, bool isBaseline)
{
    QDataStream ds(itemBlock);
    ImageItem item;
    ds >> item;

    QString prefix = pathForItem(item, isBaseline);
    qDebug() << runId << logtime() << "Received" << (isBaseline ? "baseline" : "mismatched") << "image for:" << item.scriptName << "Storing in" << prefix;

    QString dir = prefix.section(QLC('/'), 0, -2);
    QDir cwd;
    if (!cwd.exists(dir))
        cwd.mkpath(dir);
    item.image.save(prefix + QLS(FileFormat), FileFormat);

    //# Could use QSettings or XML or even DB, could use common file for whole dir or even whole storage - but for now, keep it simple
    QFile file(prefix + QLS("metadata"));
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream checkSums(&file);
    checkSums << item.imageChecksums;
    file.close();

    if (!isBaseline)
        report.addItem(pathForItem(item, true, false) + QLS(FileFormat),
                       pathForItem(item, false, false) + QLS(FileFormat),
                       item);

    QByteArray msg(isBaseline ? "New baseline image stored: " :
                                "Mismatch report: " );
    msg += BaselineServer::baseUrl();
    if (isBaseline)
        msg += pathForItem(item, true, false).toLatin1() + FileFormat;
    else
        msg += report.filePath();

    proto.sendBlock(BaselineProtocol::Ack, msg);
}


void BaselineHandler::receiveDisconnect()
{
    qDebug() << runId << logtime() << "Client disconnected.";
    report.end();
    QThread::currentThread()->exit(0);
}


void BaselineHandler::mapPlatformInfo()
{
    mapped = plat;

    // Map hostname
    QString host = plat.value(PI_HostName).section(QLC('.'), 0, 0);  // Filter away domain, if any
    if (host.isEmpty() || host == QLS("localhost")) {
        host = plat.value(PI_HostAddress);
    } else {
        //# Site specific, should be in a config file
        if (!host.startsWith(QLS("oldhcp"))) {
            // remove index postfix typical of vm hostnames
            host.remove(QRegExp(QLS("\\d+$")));
            if (host.endsWith(QLC('-')))
                host.chop(1);
        }
    }
    if (host.isEmpty())
        host = QLS("unknownhost");
    mapped.insert(PI_HostName, host);

    // Map qmakespec
    QString mkspec = plat.value(PI_QMakeSpec);
    mapped.insert(PI_QMakeSpec, mkspec.replace(QLC('/'), QLC('_')));

    // Map Qt version
    QString ver = plat.value(PI_QtVersion);
    mapped.insert(PI_QtVersion, ver.prepend(QLS("Qt-")));   //### TBD: remove patch version
}

QString BaselineHandler::pathForItem(const ImageItem &item, bool isBaseline, bool absolute)
{
    if (mapped.isEmpty())
        mapPlatformInfo();

    QString itemName = item.scriptName;
    if (itemName.contains(QLC('.')))
        itemName.replace(itemName.lastIndexOf(QLC('.')), 1, QLC('_'));
    itemName.append(QLC('_'));
    itemName.append(QString::number(item.scriptChecksum, 16).rightJustified(4, QLC('0')));

    QStringList path;
    if (absolute)
        path += BaselineServer::storagePath();
    path += QLS(isBaseline ? "baselines" : "mismatches");
    path += item.engineAsString() + QLC('_') + item.formatAsString();
    path += mapped.value(PI_QtVersion);
    path += mapped.value(PI_QMakeSpec);
    path += mapped.value(PI_HostName);
    if (!isBaseline)
        path += runId;
    path += itemName + QLC('.');

    return path.join(QLS("/"));
}


QString BaselineHandler::clearAllBaselines(const QString &context)
{
    int tot = 0;
    int failed = 0;
    QDirIterator it(BaselineServer::storagePath() + QLC('/') + context,
                    QStringList() << QLS("*.png") << QLS("*.metadata"));
    while (it.hasNext()) {
        tot++;
        if (!QFile::remove(it.next()))
            failed++;
    }
    return QString(QLS("%1 of %2 baselines cleared from context ")).arg((tot-failed)/2).arg(tot/2) + context;
}

QString BaselineHandler::updateSingleBaseline(const QString &oldBaseline, const QString &newBaseline)
{
    QString res;
    QString basePath(BaselineServer::storagePath() + QLC('/'));
    QString srcBase(basePath + newBaseline.left(newBaseline.length() - 3));
    QString dstDir(basePath + oldBaseline.left(oldBaseline.lastIndexOf(QLC('/'))));

    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(QLS("cp"), QStringList() << QLS("-f") << srcBase + QLS("png") << srcBase + QLS("metadata") << dstDir);
    proc.waitForFinished();
    if (proc.exitCode() == 0)
        res = QString("Successfully updated '%1'").arg(oldBaseline + QLS("/metadata"));
    else
        res = QString("Error updating baseline: %1<br>"
                      "Command output: <pre>%2</pre>").arg(proc.errorString(), proc.readAll().constData());

    return res;
}

QString BaselineHandler::blacklistTest(const QString &context, const QString &itemId, bool removeFromBlacklist)
{
    QFile file(BaselineServer::storagePath() + QLC('/') + context + QLS("/BLACKLIST"));
    QStringList blackList;
    if (file.open(QIODevice::ReadWrite)) {
        while (!file.atEnd())
            blackList.append(file.readLine().trimmed());

        if (removeFromBlacklist)
            blackList.removeAll(itemId);
        else if (!blackList.contains(itemId))
            blackList.append(itemId);

        file.resize(0);
        foreach (QString id, blackList)
            file.write(id.toLatin1() + '\n');
        file.close();
        return QLS(removeFromBlacklist ? "Whitelisted " : "Blacklisted ") + itemId + QLS(" in context ") + context;
    } else {
        return QLS("Unable to update blacklisted tests, failed to open ") + file.fileName();
    }
}


void BaselineHandler::testPathMapping()
{
    qDebug() << "Storage prefix:" << BaselineServer::storagePath();

    QStringList hosts;
    hosts << QLS("bq-ubuntu910-x86-01")
          << QLS("bq-ubuntu910-x86-15")
          << QLS("osl-mac-master-5.test.qt.nokia.com")
          << QLS("osl-mac-master-6.test.qt.nokia.com")
          << QLS("sv-xp-vs-010")
          << QLS("sv-xp-vs-011")
          << QLS("sv-solaris-sparc-008")
          << QLS("macbuilder-02.test.troll.no")
          << QLS("bqvm1164")
          << QLS("chimera")
          << QLS("localhost");

    ImageItem item;
    item.scriptName = QLS("arcs.qps");
    item.engine = ImageItem::Raster;
    item.renderFormat = QImage::Format_ARGB32_Premultiplied;
    item.imageChecksums << 0x0123456789abcdefULL;
    item.scriptChecksum = 0x0123;

    plat.insert(PI_QtVersion, QLS("4.8.0"));
    plat.insert(PI_BuildKey, QLS("(nobuildkey)"));
    plat.insert(PI_QMakeSpec, "linux-g++");
    foreach(const QString& host, hosts) {
        mapped.clear();
        plat.insert(PI_HostName, host);
        qDebug() << "Baseline from" << host << "->" << pathForItem(item, true);
        qDebug() << "Mismatch from" << host << "->" << pathForItem(item, false);
    }
}


QString BaselineHandler::computeMismatchScore(const QImage &baseline, const QImage &rendered)
{
    if (baseline.size() != rendered.size() || baseline.format() != rendered.format())
        return QLS("[No score, incomparable images.]");
    if (baseline.depth() != 32)
        return QLS("[Score computation not implemented for format.]");

    int w = baseline.width();
    int h = baseline.height();

    uint ncd = 0; // number of differing color pixels
    uint nad = 0; // number of differing alpha pixels
    uint scd = 0; // sum of color pixel difference
    uint sad = 0; // sum of alpha pixel difference

    for (int y=0; y<h; ++y) {
        const QRgb *bl = (const QRgb *) baseline.constScanLine(y);
        const QRgb *rl = (const QRgb *) rendered.constScanLine(y);
        for (int x=0; x<w; ++x) {
            QRgb b = bl[x];
            QRgb r = rl[x];
            if (r != b) {
                int dr = qAbs(qRed(b) - qRed(r));
                int dg = qAbs(qGreen(b) - qGreen(r));
                int db = qAbs(qBlue(b) - qBlue(r));
                int ds = dr + dg + db;
                int da = qAbs(qAlpha(b) - qAlpha(r));
                if (ds) {
                    ncd++;
                    scd += ds;
                }
                if (da) {
                    nad++;
                    sad += da;
                }
            }
        }
    }

    double pcd = 100.0 * ncd / (w*h);  // percent of pixels that differ
    double acd = ncd ? double(scd) / (3*ncd) : 0;         // avg. difference
    QString res = QString(QLS("Diffscore: %1% (Num:%2 Avg:%3)")).arg(pcd, 0, 'g', 2).arg(ncd).arg(acd, 0, 'g', 2);
    if (baseline.hasAlphaChannel()) {
        double pad = 100.0 * nad / (w*h);  // percent of pixels that differ
        double aad = nad ? double(sad) / (3*nad) : 0;         // avg. difference
        res += QString(QLS(" Alpha-diffscore: %1% (Num:%2 Avg:%3)")).arg(pad, 0, 'g', 2).arg(nad).arg(aad, 0, 'g', 2);
    }
    return res;
}
