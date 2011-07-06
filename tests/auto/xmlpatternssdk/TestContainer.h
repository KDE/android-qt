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

#ifndef PatternistSDK_TestContainer_H
#define PatternistSDK_TestContainer_H

#include "Global.h"
#include "TestItem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    /**
     * @short an abstract base class, containing
     * an appropriate implementation of TestItem for sub-classes
     * which can contain other TestItem instances.
     *
     * @ingroup PatternistSDK
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class Q_PATTERNISTSDK_EXPORT TestContainer : public TestItem
    {
    public:
        virtual ~TestContainer();
        virtual void appendChild(TreeItem *item);
        virtual TreeItem *child(const unsigned int row) const;
        virtual unsigned int childCount() const;

        /**
         * @returns the TestResults of this TestContainer's children.
         */
        virtual TestResult::List execute(const ExecutionStage stage,
                                         TestSuite *ts);

        QString title() const;
        void setTitle(const QString &title);

        virtual TreeItem::List children() const;

        /**
         * @return always 2
         */
        virtual int columnCount() const;

        virtual bool isFinalNode() const;

        virtual ResultSummary resultSummary() const;
        virtual QString description() const;
        virtual void setDescription(const QString &desc);

        /**
         * Determines whether TestContainer will delete its children upon
         * destruction. By default, it will.
         */
        void setDeleteChildren(const bool val);

        /**
         * Removes the last appended child.
         */
        void removeLast();

    protected:
        /**
         * Constructor, protected. TestContainer is an abstract class,
         * and is not ment to be instantiated, but sub classed.
         */
        TestContainer();

    private:
        TreeItem::List  m_children;
        QString         m_title;
        QString         m_description;
        bool            m_deleteChildren;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
