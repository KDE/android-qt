/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QFSFILEENGINE_P_H
#define QFSFILEENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qplatformdefs.h"
#include "QtCore/qfsfileengine.h"
#include "private/qabstractfileengine_p.h"
#include <QtCore/private/qfilesystementry_p.h>
#include <QtCore/private/qfilesystemmetadata_p.h>
#include <qhash.h>

#ifdef Q_OS_SYMBIAN
#include <f32file.h>
#endif

#ifndef QT_NO_FSFILEENGINE

QT_BEGIN_NAMESPACE

#if defined(Q_OS_WINCE_STD) && _WIN32_WCE < 0x600
#define Q_USE_DEPRECATED_MAP_API 1
#endif

class Q_AUTOTEST_EXPORT QFSFileEnginePrivate : public QAbstractFileEnginePrivate
{
    Q_DECLARE_PUBLIC(QFSFileEngine)

public:
#ifdef Q_WS_WIN
    static QString longFileName(const QString &path);
#endif

    QFileSystemEntry fileEntry;
    QIODevice::OpenMode openMode;

    bool nativeOpen(QIODevice::OpenMode openMode);
    bool openFh(QIODevice::OpenMode flags, FILE *fh);
    bool openFd(QIODevice::OpenMode flags, int fd);
    bool nativeClose();
    bool closeFdFh();
    bool nativeFlush();
    bool flushFh();
    qint64 nativeSize() const;
#ifndef Q_OS_WIN
    qint64 sizeFdFh() const;
#endif
    qint64 nativePos() const;
    qint64 posFdFh() const;
    bool nativeSeek(qint64);
    bool seekFdFh(qint64);
    qint64 nativeRead(char *data, qint64 maxlen);
    qint64 readFdFh(char *data, qint64 maxlen);
    qint64 nativeReadLine(char *data, qint64 maxlen);
    qint64 readLineFdFh(char *data, qint64 maxlen);
    qint64 nativeWrite(const char *data, qint64 len);
    qint64 writeFdFh(const char *data, qint64 len);
    int nativeHandle() const;
    bool nativeIsSequential() const;
#ifndef Q_OS_WIN
    bool isSequentialFdFh() const;
#endif

    uchar *map(qint64 offset, qint64 size, QFile::MemoryMapFlags flags);
    bool unmap(uchar *ptr);

    mutable QFileSystemMetaData metaData;

    FILE *fh;
#ifdef Q_OS_SYMBIAN
#ifdef  SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    RFile64 symbianFile;
    TInt64 symbianFilePos;
#else
    RFile symbianFile;
    
    /**
     * The cursor position in the underlying file.  This differs
     * from devicePos because the latter is updated on calls to
     * writeData, even if no data was physically transferred to
     * the file, but instead stored in the write buffer.
     * 
     * iFilePos is updated on calls to RFile::Read and
     * RFile::Write.  It is also updated on calls to seek() but
     * RFile::Seek is not called when that happens because
     * Symbian supports positioned reads and writes, saving a file
     * server call, and because Symbian does not support seeking
     * past the end of a file.  
     */
    TInt symbianFilePos;
#endif
    mutable int fileHandleForMaps;
    int getMapHandle();
#endif

#ifdef Q_WS_WIN
    HANDLE fileHandle;
    HANDLE mapHandle;
    QHash<uchar *, DWORD /* offset % AllocationGranularity */> maps;

#ifndef Q_OS_WINCE
    mutable int cachedFd;
#endif

    mutable DWORD fileAttrib;
#else
    QHash<uchar *, QPair<int /*offset % PageSize*/, size_t /*length + offset % PageSize*/> > maps;
#endif
    int fd;

    enum LastIOCommand
    {
        IOFlushCommand,
        IOReadCommand,
        IOWriteCommand
    };
    LastIOCommand  lastIOCommand;
    bool lastFlushFailed;
    bool closeFileHandle;

    mutable uint is_sequential : 2;
    mutable uint could_stat : 1;
    mutable uint tried_stat : 1;
#if !defined(Q_OS_WINCE)
    mutable uint need_lstat : 1;
    mutable uint is_link : 1;
#endif

#if defined(Q_OS_WIN)
    bool doStat(QFileSystemMetaData::MetaDataFlags flags) const;
#else
    bool doStat(QFileSystemMetaData::MetaDataFlags flags = QFileSystemMetaData::PosixStatFlags) const;
#endif
    bool isSymlink() const;

#if defined(Q_OS_WIN32)
    int sysOpen(const QString &, int flags);
#endif

protected:
    QFSFileEnginePrivate();

    void init();

    QAbstractFileEngine::FileFlags getPermissions(QAbstractFileEngine::FileFlags type) const;
};

QT_END_NAMESPACE

#endif // QT_NO_FSFILEENGINE

#endif // QFSFILEENGINE_P_H
