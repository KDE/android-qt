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
// all 5 strings share the same data
QString s1 = "abcd";
QString s2 = s1;
QString s3 = s2;
QString s4 = s3;
QString s5 = s2;
//! [0]


//! [1]
// s1, s2 and s5 share the same data, neither s3 nor s4 are shared
QString s1 = "abcd";
QString s2 = s1;
Q3DeepCopy<QString> s3 = s2;  // s3 is a deep copy of s2
QString s4 = s3;             // s4 is a deep copy of s3
QString s5 = s2;
//! [1]


//! [2]
// s1, s2 and s5 share the same data, s3 and s4 share the same data
QString s1 = "abcd";
QString s2 = s1;
QString s3 = Q3DeepCopy<QString>( s2 );  // s3 is a deep copy of s2
QString s4 = s3;                        // s4 is a shallow copy of s3
QString s5 = s2;
//! [2]


//! [3]
Q3DeepCopy<QString> global_string;  // global string data
QMutex global_mutex;               // mutex to protext global_string

...

void setGlobalString( const QString &str )
{
    global_mutex.lock();
    global_string = str;           // global_string is a deep copy of str
    global_mutex.unlock();
}

...

void MyThread::run()
{
    global_mutex.lock();
    QString str = global_string;          // str is a deep copy of global_string
    global_mutex.unlock();

    // process the string data
    ...

    // update global_string
    setGlobalString( str );
}
//! [3]
