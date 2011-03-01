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


#include <QtCore>
#include <QtTest/QtTest>

/* This test must be explicitly enabled since there are no compile tests for valgrind.h */
#ifdef QT_BUG236484
#include <valgrind/valgrind.h>
#endif

class tst_BenchlibCallgrind: public QObject
{
    Q_OBJECT

private slots:
#ifdef QT_BUG236484
    void failInChildProcess();
#endif

    void twoHundredMillionInstructions();
};

#ifdef QT_BUG236484
void tst_BenchlibCallgrind::failInChildProcess()
{
    static double f = 1.0;
    QBENCHMARK {
        for (int i = 0; i < 1000000; ++i) {
            f *= 1.1;
            if (RUNNING_ON_VALGRIND) QFAIL("Running under valgrind!");
        }
    }
}
#endif

void tst_BenchlibCallgrind::twoHundredMillionInstructions()
{
#if !defined(__GNUC__) || !defined(__i386)
    QSKIP("This test is only defined for gcc and x86.", SkipAll);
#else
    QBENCHMARK {
        __asm__ __volatile__(
            "mov $100000000,%%eax   \n"
            "LOOPTOP:               \n"
            "dec %%eax              \n"
            "jnz LOOPTOP            \n"
            : /* no output */
            : /* no input */
            : /* clobber */ "eax"
        );
    }
#endif
}

QTEST_MAIN(tst_BenchlibCallgrind)

#include "tst_benchlibcallgrind.moc"
