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

#include <QStringList>
#include <QtTest>

#include <sstream>
#include <string>
#include <vector>

class tst_QStringList: public QObject
{
    Q_OBJECT

private slots:
    void join() const;
    void join_data() const;

    void split() const;
    void split_data() const;

    void split_std() const;
    void split_std_data() const { return split_data(); }

    void split_stdw() const;
    void split_stdw_data() const { return split_data(); }

    void split_ba() const;
    void split_ba_data() const { return split_data(); }

private:
    static QStringList populateList(const int count, const QString &unit);
    static QString populateString(const int count, const QString &unit);
};

QStringList tst_QStringList::populateList(const int count, const QString &unit)
{
    QStringList retval;

    for (int i = 0; i < count; ++i)
        retval.append(unit);

    return retval;
}

QString tst_QStringList::populateString(const int count, const QString &unit)
{
    QString retval;

    for (int i = 0; i < count; ++i) {
        retval.append(unit);
        retval.append(QLatin1Char(':'));
    }

    return retval;
}

void tst_QStringList::join() const
{
    QFETCH(QStringList, input);
    QFETCH(QString, separator);

    QBENCHMARK {
        input.join(separator);
    }
}

void tst_QStringList::join_data() const
{
    QTest::addColumn<QStringList>("input");
    QTest::addColumn<QString>("separator");

    QTest::newRow("")
        << populateList(100, QLatin1String("unit"))
        << QString();

    QTest::newRow("")
        << populateList(1000, QLatin1String("unit"))
        << QString();

    QTest::newRow("")
        << populateList(10000, QLatin1String("unit"))
        << QString();

    QTest::newRow("")
        << populateList(100000, QLatin1String("unit"))
        << QString();
}

void tst_QStringList::split() const
{
    QFETCH(QString, input);
    const QChar splitChar = ':';

    QBENCHMARK {
        input.split(splitChar);
    }
}

void tst_QStringList::split_data() const
{
    QTest::addColumn<QString>("input");
    QString unit = QLatin1String("unit");
    QTest::newRow("") << populateString(10, unit);
    QTest::newRow("") << populateString(100, unit);
    QTest::newRow("") << populateString(1000, unit);
    QTest::newRow("") << populateString(10000, unit);
}

void tst_QStringList::split_std() const
{
    QFETCH(QString, input);
    const char split_char = ':';
    std::string stdinput = input.toStdString();

    QBENCHMARK {
        std::istringstream split(stdinput);
        std::vector<std::string> token;
        for (std::string each;
             std::getline(split, each, split_char);
             token.push_back(each))
            ;
    }
}

void tst_QStringList::split_stdw() const
{
    QFETCH(QString, input);
    const wchar_t split_char = ':';
    std::wstring stdinput = input.toStdWString();

    QBENCHMARK {
        std::wistringstream split(stdinput);
        std::vector<std::wstring> token;
        for (std::wstring each;
             std::getline(split, each, split_char);
             token.push_back(each))
            ;
    }
}

void tst_QStringList::split_ba() const
{
    QFETCH(QString, input);
    const char splitChar = ':';
    QByteArray ba = input.toLatin1();

    QBENCHMARK {
        ba.split(splitChar);
    }
}

QTEST_MAIN(tst_QStringList)

#include "main.moc"
