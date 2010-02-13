/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef DEBUGGER_TRK_UTILS
#define DEBUGGER_TRK_UTILS

#include "symbianutils_global.h"
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE
class QDateTime;
QT_END_NAMESPACE

namespace trk {

typedef unsigned char byte;

enum Command {
    TrkPing = 0x00,
    TrkConnect = 0x01,
    TrkDisconnect = 0x02,
    TrkVersions = 0x04,
    TrkSupported = 0x05,
    TrkCpuType = 0x06,
    TrkHostVersions = 0x09,
    TrkContinue = 0x18,
    TrkCreateItem = 0x40,
    TrkDeleteItem = 0x41,

    TrkWriteFile = 0x48,
    TrkOpenFile = 0x4a,
    TrkCloseFile = 0x4b,
    TrkInstallFile = 0x4d,
    TrkInstallFile2 = 0x4e,

    TrkNotifyAck = 0x80,
    TrkNotifyNak = 0xff,
    TrkNotifyStopped = 0x90,
    TrkNotifyException = 0x91,
    TrkNotifyInternalError = 0x92,
    TrkNotifyCreated = 0xa0,
    TrkNotifyDeleted = 0xa1,
    TrkNotifyProcessorStarted = 0xa2,
    TrkNotifyProcessorStandBy = 0xa6,
    TrkNotifyProcessorReset = 0xa7
};

inline byte extractByte(const char *data) { return *data; }
SYMBIANUTILS_EXPORT ushort extractShort(const char *data);
SYMBIANUTILS_EXPORT uint extractInt(const char *data);

SYMBIANUTILS_EXPORT QString quoteUnprintableLatin1(const QByteArray &ba);

// produces "xx xx xx "
SYMBIANUTILS_EXPORT QString stringFromArray(const QByteArray &ba, int maxLen = - 1);

enum Endianness
{
    LittleEndian,
    BigEndian,
    TargetByteOrder = BigEndian,
};

SYMBIANUTILS_EXPORT void appendShort(QByteArray *ba, ushort s, Endianness = TargetByteOrder);
SYMBIANUTILS_EXPORT void appendInt(QByteArray *ba, uint i, Endianness = TargetByteOrder);
SYMBIANUTILS_EXPORT void appendString(QByteArray *ba, const QByteArray &str, Endianness = TargetByteOrder, bool appendNullTerminator = true);

struct SYMBIANUTILS_EXPORT Library
{
    Library() {}

    QByteArray name;
    uint codeseg;
    uint dataseg;
};

struct SYMBIANUTILS_EXPORT TrkAppVersion
{
    TrkAppVersion();
    void reset();    

    int trkMajor;
    int trkMinor;
    int protocolMajor;
    int protocolMinor;
};

struct SYMBIANUTILS_EXPORT Session
{
    Session();
    void reset();
    QString deviceDescription(unsigned verbose) const;

    // Trk feedback
    byte cpuMajor;
    byte cpuMinor;
    byte bigEndian;
    byte defaultTypeSize;
    byte fpTypeSize;
    byte extended1TypeSize;
    byte extended2TypeSize;
    TrkAppVersion trkAppVersion;
    uint pid;
    uint tid;
    uint codeseg;
    uint dataseg;
    QHash<uint, uint> addressToBP;

    typedef QList<Library> Libraries;
    Libraries libraries;

    typedef uint Thread;
    typedef QList<Thread> Threads;
    Threads threads;

    // Gdb request
    uint currentThread;
    QStringList modules;
};

struct SYMBIANUTILS_EXPORT TrkResult
{
    TrkResult();
    void clear();
    QString toString() const;
    // 0 for no error.
    int errorCode() const;
    QString errorString() const;

    byte code;
    byte token;
    QByteArray data;
    QVariant cookie;
    bool isDebugOutput;
};

SYMBIANUTILS_EXPORT QByteArray errorMessage(byte code);
SYMBIANUTILS_EXPORT QByteArray hexNumber(uint n, int digits = 0);
SYMBIANUTILS_EXPORT QByteArray hexxNumber(uint n, int digits = 0); // prepends '0x', too
SYMBIANUTILS_EXPORT uint swapEndian(uint in);

} // namespace trk

#endif // DEBUGGER_TRK_UTILS
