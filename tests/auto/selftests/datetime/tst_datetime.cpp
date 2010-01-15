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


#include <QtCore>
#include <QtTest/QtTest>

#include <QDateTime>

/*!
  \internal
 */
class tst_DateTime: public QObject
{
    Q_OBJECT

private slots:
    void dateTime() const;
    void qurl() const;
    void qurl_data() const;
};

void tst_DateTime::dateTime() const
{
    const QDateTime utc(QDate(2000, 5, 3), QTime(4, 3, 4), Qt::UTC);
    const QDateTime local(QDate(2000, 5, 3), QTime(4, 3, 4), Qt::LocalTime);

    QCOMPARE(local, utc);
}

void tst_DateTime::qurl() const
{
    QFETCH(QUrl, operandA);
    QFETCH(QUrl, operandB);
    
    QCOMPARE(operandA, operandB);
}

void tst_DateTime::qurl_data() const
{
    QTest::addColumn<QUrl>("operandA");
    QTest::addColumn<QUrl>("operandB");

    QTest::newRow("") << QUrl() << QUrl();
    QTest::newRow("") << QUrl(QLatin1String("http://example.com")) << QUrl();
    QTest::newRow("") << QUrl() << QUrl(QLatin1String("http://example.com"));
    QTest::newRow("") << QUrl(QLatin1String("http://example.com")) << QUrl(QLatin1String("http://example.com"));
}

QTEST_MAIN(tst_DateTime)

#include "tst_datetime.moc"
