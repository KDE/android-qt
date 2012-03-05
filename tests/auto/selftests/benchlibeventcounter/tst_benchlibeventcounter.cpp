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

class tst_BenchlibEventCounter: public QObject
{
    Q_OBJECT

private slots:
    void events();
    void events_data();
};

void tst_BenchlibEventCounter::events()
{
    QFETCH(int, eventCount);

    QAbstractEventDispatcher* ed = QAbstractEventDispatcher::instance();
    QBENCHMARK {
        for (int i = 0; i < eventCount; ++i) {
            ed->filterEvent(0);
        }
    }
}

void tst_BenchlibEventCounter::events_data()
{
    QTest::addColumn<int>("eventCount");

    QTest::newRow("0")      << 0;
    QTest::newRow("1")      << 1;
    QTest::newRow("10")     << 10;
    QTest::newRow("100")    << 100;
    QTest::newRow("500")    << 500;
    QTest::newRow("5000")   << 5000;
    QTest::newRow("100000") << 100000;
}

int main(int argc, char** argv)
{
    TestEventDispatcher dispatcher;
    QCoreApplication app(argc, argv);
    tst_BenchlibEventCounter test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_benchlibeventcounter.moc"
