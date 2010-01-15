/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QXMLRESULTITEMS_P_H
#define QXMLRESULTITEMS_P_H

#include "qcommonvalues_p.h"
#include "qdynamiccontext_p.h"
#include "qitem_p.h"

QT_BEGIN_NAMESPACE

class QXmlResultItemsPrivate
{
public:
    inline QXmlResultItemsPrivate() : iterator(QPatternist::CommonValues::emptyIterator)
                                    , hasError(false)
    {
    }

    void setDynamicContext(const QPatternist::DynamicContext::Ptr &context)
    {
        m_context = context;
    }

    QPatternist::Item::Iterator::Ptr    iterator;
    QXmlItem                            current;
    bool                                hasError;
private:
    /**
     * We never use it. We only keep a ref to it such that it doesn't get
     * de-allocated.
     */
    QPatternist::DynamicContext::Ptr    m_context;
};

QT_END_NAMESPACE
#endif

