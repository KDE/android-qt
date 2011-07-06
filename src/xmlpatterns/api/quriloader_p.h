/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QURILOADER_P_H
#define QURILOADER_P_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtXmlPatterns/QXmlName>

#include "qnamepool_p.h"
#include "qvariableloader_p.h"

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class URILoader : public QNetworkAccessManager
    {
    public:
        URILoader(QObject *const parent,
                  const NamePool::Ptr &np,
                  const VariableLoader::Ptr &variableLoader);

        virtual QNetworkReply *createRequest(Operation op,
                                             const QNetworkRequest & req,
                                             QIODevice *outgoingData = 0);

    private:
        const QString               m_variableNS;
        const NamePool::Ptr         m_namePool;
        const VariableLoader::Ptr   m_variableLoader;

    };
}

QT_END_NAMESPACE

#endif
