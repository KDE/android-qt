/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PatternistSDK_ResultThreader_H
#define PatternistSDK_ResultThreader_H

#include <QFile>
#include <QFileInfo>
#include <QThread>

#include "TestResultHandler.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QEventLoop;

namespace QPatternistSDK
{
    /**
     * @short Reads XML in the @c XQTSResult.xsd format, as a thread, allowing
     * multiple parses to be done simultaneously.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup PatternistSDK
     */
    class Q_PATTERNISTSDK_EXPORT ResultThreader : public QThread
                                                , public TestResultHandler
    {
    public:
        enum Type
        {
            Baseline = 1,
            Result
        };

        /**
         * Creates a ResultThreader that will read @p file when run() is called.
         */
        ResultThreader(QEventLoop &ev,
                       QFile *file,
                       const Type type,
                       QObject *parent);

        /**
         * Parses the file passed in this class's constructor with this ResultHandlerTH::Item::LisT
         * as the QXmlContentHandler, and returns.
         */
        virtual void run();

        /**
         * @note Do not reimplement this function.
         * @returns whether this ResultThreader handles the baseline or the result.
         */
        Type type() const;

    private:
        Q_DISABLE_COPY(ResultThreader)

        QFile *const    m_file;
        const Type      m_type;
        QEventLoop &    m_eventLoop;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
