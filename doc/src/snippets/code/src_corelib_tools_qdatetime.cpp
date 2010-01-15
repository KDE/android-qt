/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
QDate d1(1995, 5, 17);  // May 17, 1995
QDate d2(1995, 5, 20);  // May 20, 1995
d1.daysTo(d2);          // returns 3
d2.daysTo(d1);          // returns -3
//! [0]


//! [1]
QDate date = QDate::fromString("1MM12car2003", "d'MM'MMcaryyyy");
// date is 1 December 2003
//! [1]


//! [2]
QDate date = QDate::fromString("130", "Md"); // invalid
//! [2]


//! [3]
QDate::fromString("1.30", "M.d");           // January 30 1900
QDate::fromString("20000110", "yyyyMMdd");  // January 10, 2000
QDate::fromString("20000110", "yyyyMd");    // January 10, 2000
//! [3]


//! [4]
QDate::isValid(2002, 5, 17);  // true
QDate::isValid(2002, 2, 30);  // false (Feb 30 does not exist)
QDate::isValid(2004, 2, 29);  // true (2004 is a leap year)
QDate::isValid(2000, 2, 29);  // true (2000 is a leap year)
QDate::isValid(2006, 2, 29);  // false (2006 is not a leap year)
QDate::isValid(2100, 2, 29);  // false (2100 is not a leap year)
QDate::isValid(1202, 6, 6);   // true (even though 1202 is pre-Gregorian)
//! [4]


//! [5]
QTime n(14, 0, 0);                // n == 14:00:00
QTime t;
t = n.addSecs(70);                // t == 14:01:10
t = n.addSecs(-70);               // t == 13:58:50
t = n.addSecs(10 * 60 * 60 + 5);  // t == 00:00:05
t = n.addSecs(-15 * 60 * 60);     // t == 23:00:00
//! [5]


//! [6]
QTime time = QTime::fromString("1mm12car00", "m'mm'hcarss");
// time is 12:01.00
//! [6]


//! [7]
QTime time = QTime::fromString("00:710", "hh:ms"); // invalid
//! [7]


//! [8]
QTime time = QTime::fromString("1.30", "m.s");
// time is 00:01:30.000
//! [8]


//! [9]
QTime::isValid(21, 10, 30); // returns true
QTime::isValid(22, 5,  62); // returns false
//! [9]


//! [10]
QTime t;
t.start();
some_lengthy_task();
qDebug("Time elapsed: %d ms", t.elapsed());
//! [10]


//! [11]
QDateTime now = QDateTime::currentDateTime();
QDateTime xmas(QDate(now.date().year(), 12, 25), QTime(0, 0));
qDebug("There are %d seconds to Christmas", now.secsTo(xmas));
//! [11]


//! [12]
QTime time1 = QTime::fromString("131", "HHh");
// time1 is 13:00:00
QTime time1 = QTime::fromString("1apA", "1amAM");
// time1 is 01:00:00

QDateTime dateTime2 = QDateTime::fromString("M1d1y9800:01:02",
                                            "'M'M'd'd'y'yyhh:mm:ss");
// dateTime is 1 January 1998 00:01:02
//! [12]


//! [13]
QDateTime dateTime = QDateTime::fromString("130", "Mm"); // invalid
//! [13]


//! [14]
QDateTime dateTime = QDateTime::fromString("1.30.1", "M.d.s");
// dateTime is January 30 in 1900 at 00:00:01.
//! [14]
