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


#include <QtTest/QtTest>

#ifdef QTEST_XMLPATTERNS
#include <QtXmlPatterns/QXmlNamePool>
#include <QtXmlPatterns/QXmlName>

/*!
 \class tst_QXmlNamePool
 \internal
 \since 4.4
 \brief Tests class QXmlNamePool.

 */
class tst_QXmlNamePool : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void constCorrectness() const;
    void size() const;
    void assignmentOperator() const;
};

/*!
  Check that functions have the correct const qualification.
 */
void tst_QXmlNamePool::constCorrectness() const
{
    /* No functions are const in the name pool, so do nothing. */
}

void tst_QXmlNamePool::size() const
{
    QVERIFY2(sizeof(QXmlNamePool) == sizeof(void *), "QXmlNamePool should have a d-pointer.");
}

void tst_QXmlNamePool::assignmentOperator() const
{
    // TODO
}

QTEST_MAIN(tst_QXmlNamePool)

#include "tst_qxmlnamepool.moc"
#else
QTEST_NOOP_MAIN
#endif
// vim: et:ts=4:sw=4:sts=4
