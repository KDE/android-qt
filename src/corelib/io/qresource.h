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

#ifndef QRESOURCE_H
#define QRESOURCE_H

#include <QtCore/qstring.h>
#include <QtCore/qlocale.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QResourcePrivate;

class Q_CORE_EXPORT QResource
{
public:
    QResource(const QString &file=QString(), const QLocale &locale=QLocale());
    ~QResource();

    void setFileName(const QString &file);
    QString fileName() const;
    QString absoluteFilePath() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

    bool isValid() const;

    bool isCompressed() const;
    qint64 size() const;
    const uchar *data() const;

    static void addSearchPath(const QString &path);
    static QStringList searchPaths();

    static bool registerResource(const QString &rccFilename, const QString &resourceRoot=QString());
    static bool unregisterResource(const QString &rccFilename, const QString &resourceRoot=QString());

    static bool registerResource(const uchar *rccData, const QString &resourceRoot=QString());
    static bool unregisterResource(const uchar *rccData, const QString &resourceRoot=QString());

protected:
    friend class QResourceFileEngine;
    friend class QResourceFileEngineIterator;
    bool isDir() const;
    inline bool isFile() const { return !isDir(); }
    QStringList children() const;

protected:
    QScopedPointer<QResourcePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(QResource)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QRESOURCE_H
