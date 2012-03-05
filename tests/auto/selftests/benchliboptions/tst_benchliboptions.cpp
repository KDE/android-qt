/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QtCore>
#include <QtTest/QtTest>

/* Custom event dispatcher to ensure we don't receive any spontaneous events */
class TestEventDispatcher : public QAbstractEventDispatcher
{
    Q_OBJECT

public:
    TestEventDispatcher(QObject* parent =0)
        : QAbstractEventDispatcher(parent)
    {}
    void flush() {}
    bool hasPendingEvents() { return false; }
    void interrupt() {}
    bool processEvents(QEventLoop::ProcessEventsFlags) { return false; }
    void registerSocketNotifier(QSocketNotifier*) {}
    int registerTimer(int,QObject*) { return -1; }
    void registerTimer(int,int,QObject*) {}
    QList<TimerInfo> registeredTimers(QObject*) const { return QList<TimerInfo>(); }
    void unregisterSocketNotifier(QSocketNotifier*) {}
    bool unregisterTimer(int) { return false; }
    bool unregisterTimers(QObject*) { return false; }
    void wakeUp() {}
};

class tst_BenchlibOptions: public QObject
{
    Q_OBJECT

private slots:
    void threeEvents();
};

class tst_BenchlibFifteenIterations : public tst_BenchlibOptions
{ Q_OBJECT };
class tst_BenchlibOneHundredMinimum : public tst_BenchlibOptions
{ Q_OBJECT };

void tst_BenchlibOptions::threeEvents()
{
    QAbstractEventDispatcher* ed = QAbstractEventDispatcher::instance();
    QBENCHMARK {
        ed->filterEvent(0);
        ed->filterEvent(0);
        ed->filterEvent(0);
    }
}

int main(int argc, char** argv)
{
    int ret = 0;

    TestEventDispatcher dispatcher;
    QCoreApplication app(argc, argv);

    /* Run with no special arguments. */
    {
        tst_BenchlibOptions test;
        ret += QTest::qExec(&test, argc, argv);
    }

    /* Run with an exact number of iterations. */
    {
        QVector<char const*> args;
        for (int i = 0; i < argc; ++i) args << argv[i];
        args << "-iterations";
        args << "15";
        tst_BenchlibFifteenIterations test;
        ret += QTest::qExec(&test, args.count(), const_cast<char**>(args.data()));
    }

    /*
        Run until getting a value of at least 100.
    */
    {
        QVector<char const*> args;
        for (int i = 0; i < argc; ++i) args << argv[i];
        args << "-minimumvalue";
        args << "100";
        tst_BenchlibOneHundredMinimum test;
        ret += QTest::qExec(&test, args.count(), const_cast<char**>(args.data()));
    }

    return ret;
}

#include "tst_benchliboptions.moc"
