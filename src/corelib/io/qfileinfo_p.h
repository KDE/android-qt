/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QFILEINFO_P_H
#define QFILEINFO_P_H

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

#include "qfileinfo.h"

QT_BEGIN_NAMESPACE

class QFileInfoPrivate
{
public:
    QFileInfoPrivate(const QFileInfo *copy=0);
    ~QFileInfoPrivate();

    void initFileEngine(const QString &);

    enum Access {
        ReadAccess,
        WriteAccess,
        ExecuteAccess
    };
    bool hasAccess(Access access) const;

    uint getFileFlags(QAbstractFileEngine::FileFlags) const;
    QDateTime &getFileTime(QAbstractFileEngine::FileTime) const;
    QString getFileName(QAbstractFileEngine::FileName) const;

    enum { CachedFileFlags=0x01, CachedLinkTypeFlag=0x02, CachedBundleTypeFlag=0x04,
           CachedMTime=0x10, CachedCTime=0x20, CachedATime=0x40,
           CachedSize =0x08 };
    struct Data {
        inline Data()
            : ref(1), fileEngine(0), cache_enabled(1), fileSize(0)
        { clear(); }
        inline Data(const Data &copy)
            : ref(1), fileEngine(QAbstractFileEngine::create(copy.fileName)),
              fileName(copy.fileName), cache_enabled(copy.cache_enabled), fileSize(copy.fileSize)
        { clear(); }
        inline ~Data() { delete fileEngine; }
        inline void clearFlags() {
            fileFlags = 0;
            cachedFlags = 0;
            if (fileEngine)
                (void)fileEngine->fileFlags(QFSFileEngine::Refresh);
        }
        inline void clear() {
            clearFlags();
            for (int i = QAbstractFileEngine::NFileNames - 1 ; i >= 0 ; --i)
                fileNames[i].clear();
        }
        mutable QAtomicInt ref;

        QAbstractFileEngine *fileEngine;
        mutable QString fileName;
        mutable QString fileNames[QAbstractFileEngine::NFileNames];

        mutable uint cachedFlags : 31;
        mutable uint cache_enabled : 1;
        mutable uint fileFlags;
        mutable qint64 fileSize;
        mutable QDateTime fileTimes[3];
        inline bool getCachedFlag(uint c) const
        { return cache_enabled ? (cachedFlags & c) : 0; }
        inline void setCachedFlag(uint c)
        { if (cache_enabled) cachedFlags |= c; }
    } *data;
    inline void reset() {
        detach();
        data->clear();
    }
    void detach();
};

QT_END_NAMESPACE

#endif // QFILEINFO_P_H
