/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PatternistSDK_TreeSortFilter_H
#define PatternistSDK_TreeSortFilter_H

#include <QSortFilterProxyModel>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    /**
     * @short A sort & filter model for hierarchical item models.
     *
     * Features:
     * - When sorting, numbers are treated as a whole instead of on a
     *   character-per-character basis. For example, @c myFile-10 is sorted after @c myFile-9.
     * - When filtering, it behaves as usually is expected when the item model is hierarchical. That is,
     *   an item is shown if it matches or any of its children matches.
     *
     * @image html TreeSortFilter.png "TreeSortFilter in action on a QTreeView."
     *
     * @ingroup PatternistSDK
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class TreeSortFilter : public QSortFilterProxyModel
    {
    public:
        /**
         * Creates a TreeSortFilter.
         *
         * @param parent the parent. Must not be @c null.
         */
        TreeSortFilter(QObject *parent);

    protected:
        /**
         * Compares @p left and @p right. They are treated as QStrings.
         */
        virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

        /**
         * Overridden to implement filtering.
         */
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    private:
        inline bool numericLessThan(const QString &l, const QString &r) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
