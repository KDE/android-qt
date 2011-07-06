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

#ifndef PatternistSDK_ErrorHandler_H
#define PatternistSDK_ErrorHandler_H

#include "Global.h"
#include "qabstractmessagehandler.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template<typename T> class QList;

namespace QPatternistSDK
{
    /**
     * @short A MessageHandler which
     * accumulates all its received ErrorHandler::Message instances
     * in a list, retrievable via messages().
     *
     * Thus, ErrorHandler does not report errors, but collects them
     * and allows easy access to them.
     *
     * @ingroup PatternistSDK
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class Q_PATTERNISTSDK_EXPORT ErrorHandler : public QAbstractMessageHandler
    {
    public:
        class Message
        {
        public:
            typedef QList<Message> List;

            QString description() const
            {
                return m_description;
            }

            void setDescription(const QString &desc)
            {
                m_description = desc;
            }

            void setIdentifier(const QUrl &newId)
            {
                m_identifier = newId;
            }

            QUrl identifier() const
            {
                return m_identifier;
            }

            QtMsgType type() const
            {
                return m_type;
            }

            void setType(const QtMsgType t)
            {
                m_type = t;
            }

        private:
            QUrl        m_identifier;
            QtMsgType   m_type;
            QString     m_description;
        };

        /**
         * Clears all accumulated errors.
         */
        void reset();

        Message::List messages() const;

        /**
         * Calling this function causes all Qt's internal debug messages to be
         * sent to @p handler. If @p handler is @c null, Qt's default message
         * handler is re-installed. In other words, via an internal proxy function,
         * it installs @p handler as Qt's message handler.
         *
         * If @p handler is heap allocated, it will be leaked.
         *
         * @see qInstallMsgHandler()
         */
        static void installQtMessageHandler(ErrorHandler *const handler);

        static ErrorHandler *handler;

    protected:
        virtual void handleMessage(QtMsgType type,
                                   const QString &description,
                                   const QUrl &identifier = QUrl(),
                                   const QSourceLocation &sourceLocation = QSourceLocation());

    private:
        ErrorHandler::Message::List m_messages;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
