/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QIODEVICEDELEGATE_P_H
#define QIODEVICEDELEGATE_P_H

#include <QtCore/QTimer>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * This is read-only currently.
     */
    class QIODeviceDelegate : public QNetworkReply
    {
        Q_OBJECT
    public:
        QIODeviceDelegate(QIODevice *const source);

        virtual void abort();

        virtual bool atEnd() const;
        virtual qint64 bytesAvailable() const;
        virtual qint64 bytesToWrite() const;
        virtual bool canReadLine() const;
        virtual void close();
        virtual bool isSequential() const;
        virtual bool open(OpenMode mode);
        virtual qint64 pos() const;
        virtual bool reset();
        virtual bool seek(qint64 pos);
        virtual qint64 size() const;
        virtual bool waitForBytesWritten(int msecs);
        virtual bool waitForReadyRead(int msecs);

    protected:
        virtual qint64 readData(char *data, qint64 maxSize);

    private Q_SLOTS:
        void networkTimeout();
    private:
        enum
        {
            /**
             * 20 seconds expressed in milliseconds.
             */
            Timeout = 20000
        };

        QIODevice *const m_source;
        QTimer m_timeout;
    };
}

QT_END_NAMESPACE

#endif
