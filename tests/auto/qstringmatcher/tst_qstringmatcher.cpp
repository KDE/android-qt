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


#include <QtTest/QtTest>
#include <qstringmatcher.h>

class tst_QStringMatcher : public QObject
{
    Q_OBJECT

private slots:
    void qstringmatcher_data();
    void qstringmatcher();
    void caseSensitivity();
    void indexIn_data();
    void indexIn();
    void setCaseSensitivity_data();
    void setCaseSensitivity();
    void assignOperator();
};

// Subclass that exposes the protected functions.
class SubQStringMatcher : public QStringMatcher
{
public:
};

void tst_QStringMatcher::qstringmatcher_data()
{
}

void tst_QStringMatcher::qstringmatcher()
{
    SubQStringMatcher matcher;
    QCOMPARE(matcher.caseSensitivity(), Qt::CaseSensitive);
    QCOMPARE(matcher.indexIn("foo", 1), 1);
    QCOMPARE(matcher.pattern(), QString());
}

// public Qt::CaseSensitivity caseSensitivity() const
void tst_QStringMatcher::caseSensitivity()
{
    SubQStringMatcher matcher;

    matcher.setCaseSensitivity(Qt::CaseSensitive);
    QCOMPARE(matcher.caseSensitivity(), Qt::CaseSensitive);
    matcher.setCaseSensitivity(Qt::CaseInsensitive);
    QCOMPARE(matcher.caseSensitivity(), Qt::CaseInsensitive);
}

void tst_QStringMatcher::indexIn_data()
{
    QTest::addColumn<QString>("needle");
    QTest::addColumn<QString>("haystack");
    QTest::addColumn<int>("from");
    QTest::addColumn<int>("indexIn");
    QTest::newRow("empty") << QString() << QString("foo") << 0 << 0;
    QTest::newRow("empty") << QString() << QString("foo") << 10 << -1;
    QTest::newRow("empty") << QString() << QString("foo") << -10 << 0;

    QTest::newRow("simple") << QString("a") << QString("foo") << 0 << -1;
    QTest::newRow("simple") << QString("a") << QString("bar") << 0 << 1;
    QTest::newRow("harder") << QString("foo") << QString("slkdf sldkjf slakjf lskd ffools ldjf") << 0 << 26;
    QTest::newRow("harder") << QString("foo") << QString("slkdf sldkjf slakjf lskd ffools ldjf") << 20 << 26;
    QTest::newRow("harder") << QString("foo") << QString("slkdf sldkjf slakjf lskd ffools ldjf") << 26 << 26;
    QTest::newRow("harder") << QString("foo") << QString("slkdf sldkjf slakjf lskd ffools ldjf") << 27 << -1;
}

void tst_QStringMatcher::indexIn()
{
    QFETCH(QString, needle);
    QFETCH(QString, haystack);
    QFETCH(int, from);
    QFETCH(int, indexIn);

    SubQStringMatcher matcher;
    matcher.setPattern(needle);

    QCOMPARE(matcher.indexIn(haystack, from), indexIn);
}

void tst_QStringMatcher::setCaseSensitivity_data()
{
    QTest::addColumn<QString>("needle");
    QTest::addColumn<QString>("haystack");
    QTest::addColumn<int>("from");
    QTest::addColumn<int>("indexIn");
    QTest::addColumn<int>("cs");

    QTest::newRow("overshot") << QString("foo") << QString("baFooz foo bar") << 14 << -1 << (int) Qt::CaseSensitive;
    QTest::newRow("sensitive") << QString("foo") << QString("baFooz foo bar") << 1 << 7 << (int) Qt::CaseSensitive;
    QTest::newRow("insensitive") << QString("foo") << QString("baFooz foo bar") << 1 << 2 << (int) Qt::CaseInsensitive;
}

void tst_QStringMatcher::setCaseSensitivity()
{
    QFETCH(QString, needle);
    QFETCH(QString, haystack);
    QFETCH(int, from);
    QFETCH(int, indexIn);
    QFETCH(int, cs);

    SubQStringMatcher matcher;
    matcher.setPattern(needle);
    matcher.setCaseSensitivity(static_cast<Qt::CaseSensitivity> (cs));

    QCOMPARE(matcher.indexIn(haystack, from), indexIn);
}

void tst_QStringMatcher::assignOperator()
{
    QString needle("d");
    QString hayStack("abcdef");
    QStringMatcher m1(needle);
    QCOMPARE(m1.indexIn(hayStack), 3);

    QStringMatcher m2 = m1;
    QCOMPARE(m2.pattern(), needle);
    QCOMPARE(m2.indexIn(hayStack), 3);
}

QTEST_MAIN(tst_QStringMatcher)
#include "tst_qstringmatcher.moc"

