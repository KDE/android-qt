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
#include <QCoreApplication>
#include <QDebug>
#include <QtTest/QtTest>

#include <qfuture.h>
#include "../qfuture/versioncheck.h"
#include <qfuturewatcher.h>
#include <qtconcurrentrun.h>
#include <qtconcurrentmap.h>

#ifndef QT_NO_CONCURRENT_TEST
#include <private/qfutureinterface_p.h>

using namespace QtConcurrent;

#include <QtTest/QtTest>

//#define PRINT

class tst_QFutureWatcher: public QObject
{
    Q_OBJECT
private slots:
    void startFinish();
    void progressValueChanged();
    void canceled();
    void resultAt();
    void resultReadyAt();
    void futureSignals();
    void watchFinishedFuture();
    void watchCanceledFuture();
    void disconnectRunningFuture();
    void toMuchProgress();
    void progressText();
    void sharedFutureInterface();
    void changeFuture();
    void cancelEvents();
    void pauseEvents();
    void finishedState();
    void throttling();
    void incrementalMapResults();
    void incrementalFilterResults();
    void qfutureSynchornizer();
};

QTEST_MAIN(tst_QFutureWatcher)

void sleeper()
{
    QTest::qSleep(100);
}

void tst_QFutureWatcher::startFinish()
{
    QFutureWatcher<void> futureWatcher;

    QSignalSpy started(&futureWatcher, SIGNAL(started()));
    QSignalSpy finished(&futureWatcher, SIGNAL(finished()));

    futureWatcher.setFuture(QtConcurrent::run(sleeper));
    QTest::qWait(10); // spin the event loop to deliver queued signals.
    QCOMPARE(started.count(), 1);
    QCOMPARE(finished.count(), 0);
    futureWatcher.future().waitForFinished();
    QTest::qWait(10);
    QCOMPARE(started.count(), 1);
    QCOMPARE(finished.count(), 1);
}

void mapSleeper(int &)
{
    QTest::qSleep(100);
}

QSet<int> progressValues;
QSet<QString> progressTexts;
QMutex mutex;
class ProgressObject : public QObject
{
Q_OBJECT
public slots:
    void printProgress(int);
    void printText(const QString &text);
    void registerProgress(int);
    void registerText(const QString &text);
};

void ProgressObject::printProgress(int progress)
{
    qDebug() << "thread" << QThread::currentThread() << "reports progress" << progress;
}

void ProgressObject::printText(const QString &text)
{
    qDebug() << "thread" << QThread::currentThread() << "reports progress text" << text;
}

void ProgressObject::registerProgress(int progress)
{
    QTest::qSleep(1);
    progressValues.insert(progress);
}

void ProgressObject::registerText(const QString &text)
{
    QTest::qSleep(1);
    progressTexts.insert(text);
}


QList<int> createList(int listSize)
{
    QList<int> list;
    for (int i = 0; i < listSize; ++i) {
        list.append(i);
    }
    return list;
}

void tst_QFutureWatcher::progressValueChanged()
{
#ifdef PRINT
    qDebug() << "main thread" << QThread::currentThread();
#endif

    progressValues.clear();
    const int listSize = 20;
    QList<int> list = createList(listSize);

    QFutureWatcher<void> futureWatcher;
    ProgressObject progressObject;
    QObject::connect(&futureWatcher, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
#ifdef PRINT
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &progressObject, SLOT(printProgress(int)), Qt::DirectConnection );
#endif
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &progressObject, SLOT(registerProgress(int)));

    futureWatcher.setFuture(QtConcurrent::map(list, mapSleeper));

    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());
    futureWatcher.disconnect();
    QVERIFY(progressValues.contains(0));
    QVERIFY(progressValues.contains(listSize));
}

class CancelObject : public QObject
{
Q_OBJECT
public:
    bool wasCanceled;
    CancelObject() : wasCanceled(false) {};
public slots:
    void cancel();
};

void CancelObject::cancel()
{
#ifdef PRINT
    qDebug() << "thread" << QThread::currentThread() << "reports canceled";
#endif
    wasCanceled = true;
}

void tst_QFutureWatcher::canceled()
{
    const int listSize = 20;
    QList<int> list = createList(listSize);

    QFutureWatcher<void> futureWatcher;
    QFuture<void> future;
    CancelObject cancelObject;

    QObject::connect(&futureWatcher, SIGNAL(canceled()), &cancelObject, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(canceled()),
        &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);

    future = QtConcurrent::map(list, mapSleeper);
    futureWatcher.setFuture(future);
    futureWatcher.cancel();
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(future.isCanceled());
    QVERIFY(cancelObject.wasCanceled);
    futureWatcher.disconnect();
    future.waitForFinished();
}

class IntTask : public RunFunctionTask<int>
{
public:
    void runFunctor()
    {
        result = 10;
    }
};

void tst_QFutureWatcher::resultAt()
{
    QFutureWatcher<int> futureWatcher;
    futureWatcher.setFuture((new IntTask())->start());
    futureWatcher.waitForFinished();
    QCOMPARE(futureWatcher.result(), 10);
    QCOMPARE(futureWatcher.resultAt(0), 10);
}

void tst_QFutureWatcher::resultReadyAt()
{
    QFutureWatcher<int> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(resultReadyAt(int)), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);

    QFuture<int> future = (new IntTask())->start();
    futureWatcher.setFuture(future);

    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!QTestEventLoop::instance().timeout());

    // Setting the future again should give us another signal.
    // (this is to prevent the race where the task associated
    // with the future finishes before setFuture is called.)
    futureWatcher.setFuture(QFuture<int>());
    futureWatcher.setFuture(future);

    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!QTestEventLoop::instance().timeout());
}

class SignalSlotObject : public QObject
{
Q_OBJECT

signals:
    void cancel();

public slots:
    void started()
    {
        qDebug() << "started called";
    }

    void finished()
    {
        qDebug() << "finished called";
    }

    void canceled()
    {
        qDebug() << "canceled called";
    }

#ifdef PRINT
    void resultReadyAt(int index)
    {
        qDebug() << "result" << index << "ready";
    }
#else
    void resultReadyAt(int) { }
#endif
    void progressValueChanged(int progress)
    {
        qDebug() << "progress" << progress;
    }

    void progressRangeChanged(int min, int max)
    {
        qDebug() << "progress range" << min << max;
    }

};

void tst_QFutureWatcher::futureSignals()
{
    {
        QFutureInterface<int> a;
        QFutureWatcher<int> f;

        SignalSlotObject object;
#ifdef PRINT
        connect(&f, SIGNAL(finished()), &object, SLOT(finished()));
        connect(&f, SIGNAL(progressValueChanged(int)), &object, SLOT(progressValueChanged(int)));
#endif
        // must connect to resultReadyAt so that the watcher can detect the connection
        // (QSignalSpy does not trigger it.)
        connect(&f, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
        a.reportStarted();
        f.setFuture(a.future());

        QSignalSpy progressSpy(&f, SIGNAL(progressValueChanged(int)));
        const int progress = 1;
        a.setProgressValue(progress);
        QTest::qWait(10);
        QCOMPARE(progressSpy.count(), 2);
        QCOMPARE(progressSpy.takeFirst().at(0).toInt(), 0);
        QCOMPARE(progressSpy.takeFirst().at(0).toInt(), 1);

        QSignalSpy finishedSpy(&f, SIGNAL(finished()));
        QSignalSpy resultReadySpy(&f, SIGNAL(resultReadyAt(int)));

        const int result = 10;
        a.reportResult(&result);
        QTest::qWait(10);
        QCOMPARE(resultReadySpy.count(), 1);
        a.reportFinished(&result);
        QTest::qWait(10);

        QCOMPARE(resultReadySpy.count(), 2);
        QCOMPARE(resultReadySpy.takeFirst().at(0).toInt(), 0); // check the index
        QCOMPARE(resultReadySpy.takeFirst().at(0).toInt(), 1);

        QCOMPARE(finishedSpy.count(), 1);
    }
}

void tst_QFutureWatcher::watchFinishedFuture()
{
    QFutureInterface<int> iface;
    iface.reportStarted();

    QFuture<int> f = iface.future();

    int value = 100;
    iface.reportFinished(&value);

    QFutureWatcher<int> watcher;

    SignalSlotObject object;
#ifdef PRINT
    connect(&watcher, SIGNAL(started()), &object, SLOT(started()));
    connect(&watcher, SIGNAL(canceled()), &object, SLOT(canceled()));
    connect(&watcher, SIGNAL(finished()), &object, SLOT(finished()));
    connect(&watcher, SIGNAL(progressValueChanged(int)), &object, SLOT(progressValueChanged(int)));
    connect(&watcher, SIGNAL(progressRangeChanged(int, int)), &object, SLOT(progressRangeChanged(int, int)));
#endif
    connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));

    QSignalSpy startedSpy(&watcher, SIGNAL(started()));
    QSignalSpy finishedSpy(&watcher, SIGNAL(finished()));
    QSignalSpy resultReadySpy(&watcher, SIGNAL(resultReadyAt(int)));
    QSignalSpy canceledSpy(&watcher, SIGNAL(canceled()));

    watcher.setFuture(f);
    QTest::qWait(10);

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultReadySpy.count(), 1);
    QCOMPARE(canceledSpy.count(), 0);
}

void tst_QFutureWatcher::watchCanceledFuture()
{
    QFuture<int> f;
    QFutureWatcher<int> watcher;

    SignalSlotObject object;
#ifdef PRINT
    connect(&watcher, SIGNAL(started()), &object, SLOT(started()));
    connect(&watcher, SIGNAL(canceled()), &object, SLOT(canceled()));
    connect(&watcher, SIGNAL(finished()), &object, SLOT(finished()));
    connect(&watcher, SIGNAL(progressValueChanged(int)), &object, SLOT(progressValueChanged(int)));
    connect(&watcher, SIGNAL(progressRangeChanged(int, int)), &object, SLOT(progressRangeChanged(int, int)));
#endif
    connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));

    QSignalSpy startedSpy(&watcher, SIGNAL(started()));
    QSignalSpy finishedSpy(&watcher, SIGNAL(finished()));
    QSignalSpy resultReadySpy(&watcher, SIGNAL(resultReadyAt(int)));
    QSignalSpy canceledSpy(&watcher, SIGNAL(canceled()));

    watcher.setFuture(f);
    QTest::qWait(10);

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultReadySpy.count(), 0);
    QCOMPARE(canceledSpy.count(), 1);
}

void tst_QFutureWatcher::disconnectRunningFuture()
{
    QFutureInterface<int> a;
    a.reportStarted();

    QFuture<int> f = a.future();
    QFutureWatcher<int> *watcher = new QFutureWatcher<int>();
    watcher->setFuture(f);

    SignalSlotObject object;
    connect(watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));

    QSignalSpy finishedSpy(watcher, SIGNAL(finished()));
    QSignalSpy resultReadySpy(watcher, SIGNAL(resultReadyAt(int)));

    const int result = 10;
    a.reportResult(&result);
    QTest::qWait(10);
    QCOMPARE(resultReadySpy.count(), 1);

    delete watcher;

    a.reportResult(&result);
    QTest::qWait(10);
    QCOMPARE(resultReadySpy.count(), 1);

    a.reportFinished(&result);
    QTest::qWait(10);
    QCOMPARE(finishedSpy.count(), 0);
}

const int maxProgress = 100000;
class ProgressEmitterTask : public RunFunctionTask<void>
{
public:
    void runFunctor()
    {
        setProgressRange(0, maxProgress);
        for (int p = 0; p <= maxProgress; ++p)
            setProgressValue(p);
    }
};

void tst_QFutureWatcher::toMuchProgress()
{
    progressValues.clear();
    ProgressObject o;

    QFutureWatcher<void> f;
    f.setFuture((new ProgressEmitterTask())->start());
    QObject::connect(&f, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
#ifdef PRINT
    QObject::connect(&f, SIGNAL(progressValueChanged(int)), &o, SLOT(printProgress(int)));
#endif
    QObject::connect(&f, SIGNAL(progressValueChanged(int)), &o, SLOT(registerProgress(int)));

    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(progressValues.contains(maxProgress));
}

template <typename T>
class ProgressTextTask : public RunFunctionTask<T>
{
public:
    void runFunctor()
    {
        while (this->isProgressUpdateNeeded() == false)
            QTest::qSleep(1);
        this->setProgressValueAndText(1, QLatin1String("Foo 1"));

        while (this->isProgressUpdateNeeded() == false)
            QTest::qSleep(1);
        this->setProgressValueAndText(2, QLatin1String("Foo 2"));

        while (this->isProgressUpdateNeeded() == false)
            QTest::qSleep(1);
        this->setProgressValueAndText(3, QLatin1String("Foo 3"));
    }
};

void tst_QFutureWatcher::progressText()
{
    {   // instantiate API for T=int and T=void.
        ProgressTextTask<int> a;
        ProgressTextTask<void> b;
    }
    {
        progressValues.clear();
        progressTexts.clear();
        QFuture<int> f = ((new ProgressTextTask<int>())->start());
        QFutureWatcher<int> watcher;
        ProgressObject o;
        QObject::connect(&watcher, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
#ifdef PRINT
        QObject::connect(&watcher, SIGNAL(progressValueChanged(int)), &o, SLOT(printProgress(int)));
        QObject::connect(&watcher, SIGNAL(progressTextChanged(const QString &)), &o, SLOT(printText(const QString &)));
#endif
        QObject::connect(&watcher, SIGNAL(progressValueChanged(int)), &o, SLOT(registerProgress(int)));
        QObject::connect(&watcher, SIGNAL(progressTextChanged(const QString &)), &o, SLOT(registerText(const QString &)));

        watcher.setFuture(f);
        QTestEventLoop::instance().enterLoop(5);
        QVERIFY(!QTestEventLoop::instance().timeout());

        QCOMPARE(f.progressText(), QLatin1String("Foo 3"));
        QCOMPARE(f.progressValue(), 3);
        QVERIFY(progressValues.contains(1));
        QVERIFY(progressValues.contains(2));
        QVERIFY(progressValues.contains(3));
        QVERIFY(progressTexts.contains(QLatin1String("Foo 1")));
        QVERIFY(progressTexts.contains(QLatin1String("Foo 2")));
        QVERIFY(progressTexts.contains(QLatin1String("Foo 3")));
    }
}

template <typename T>
void callInterface(T &obj)
{
    obj.progressValue();
    obj.progressMinimum();
    obj.progressMaximum();
    obj.progressText();

    obj.isStarted();
    obj.isFinished();
    obj.isRunning();
    obj.isCanceled();
    obj.isPaused();

    obj.cancel();
    obj.pause();
    obj.resume();
    obj.togglePaused();
    obj.waitForFinished();

    const T& objConst = obj;
    objConst.progressValue();
    objConst.progressMinimum();
    objConst.progressMaximum();
    objConst.progressText();

    objConst.isStarted();
    objConst.isFinished();
    objConst.isRunning();
    objConst.isCanceled();
    objConst.isPaused();
}

template <typename T>
void callInterface(const T &obj)
{
    obj.result();
    obj.resultAt(0);
}


// QFutureWatcher and QFuture has a similar interface. Test
// that the functions we want ot have in both are actually
// there.
void tst_QFutureWatcher::sharedFutureInterface()
{
    QFutureInterface<int> iface;
    iface.reportStarted();

    QFuture<int> intFuture = iface.future();

    int value = 0;
    iface.reportFinished(&value);

    QFuture<void> voidFuture;
    QFutureWatcher<int> intWatcher;
    intWatcher.setFuture(intFuture);
    QFutureWatcher<void> voidWatcher;

    callInterface(intFuture);
    callInterface(voidFuture);
    callInterface(intWatcher);
    callInterface(voidWatcher);

    callInterface(intFuture);
    callInterface(intWatcher);
}

void tst_QFutureWatcher::changeFuture()
{
    QFutureInterface<int> iface;
    iface.reportStarted();

    QFuture<int> a = iface.future();

    int value = 0;
    iface.reportFinished(&value);

    QFuture<int> b;

    QFutureWatcher<int> watcher;

    SignalSlotObject object;
    connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
    QSignalSpy resultReadySpy(&watcher, SIGNAL(resultReadyAt(int)));

    watcher.setFuture(a); // Watch 'a' which will genere a resultReady event.
    watcher.setFuture(b); // But oh no! we're switching to another future
    QTest::qWait(10);     // before the event gets delivered.

    QCOMPARE(resultReadySpy.count(), 0);

    watcher.setFuture(a);
    watcher.setFuture(b);
    watcher.setFuture(a); // setting it back gets us one event, not two.
    QTest::qWait(10);

    QCOMPARE(resultReadySpy.count(), 1);
}

// Test that events aren't delivered from canceled futures
void tst_QFutureWatcher::cancelEvents()
{
    QFutureInterface<int> iface;
    iface.reportStarted();

    QFuture<int> a = iface.future();

    int value = 0;
    iface.reportFinished(&value);

    QFutureWatcher<int> watcher;

    SignalSlotObject object;
    connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
    QSignalSpy resultReadySpy(&watcher, SIGNAL(resultReadyAt(int)));

    watcher.setFuture(a);
    watcher.cancel();

    QTest::qWait(10);

    QCOMPARE(resultReadySpy.count(), 0);
}

// Tests that events from paused futures are saved and
// delivered on resume.
void tst_QFutureWatcher::pauseEvents()
{
    {
        QFutureInterface<int> iface;
        iface.reportStarted();

        QFuture<int> a = iface.future();

        int value = 0;
        iface.reportFinished(&value);

        QFutureWatcher<int> watcher;

        SignalSlotObject object;
        connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
        QSignalSpy resultReadySpy(&watcher, SIGNAL(resultReadyAt(int)));

        watcher.setFuture(a);
        watcher.pause();

        QTest::qWait(10);
        QCOMPARE(resultReadySpy.count(), 0);

        watcher.resume();
        QTest::qWait(10);
        QCOMPARE(resultReadySpy.count(), 1);
    }
    {
        QFutureInterface<int> iface;
        iface.reportStarted();

        QFuture<int> a = iface.future();

        int value = 0;
        iface.reportFinished(&value);

        QFutureWatcher<int> watcher;

        SignalSlotObject object;
        connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
        QSignalSpy resultReadySpy(&watcher, SIGNAL(resultReadyAt(int)));

        watcher.setFuture(a);
        a.pause();

        QFuture<int> b;
        watcher.setFuture(b); // If we watch b instead, resuming a
        a.resume();           // should give us no results.

        QTest::qWait(10);
        QCOMPARE(resultReadySpy.count(), 0);
    }
}

// Test that the finished state for the watcher gets
// set when the finished event is delivered.
// This means it will lag the finished state for the future,
// but makes it more useful.
void tst_QFutureWatcher::finishedState()
{
    QFutureInterface<int> iface;
    iface.reportStarted();
    QFuture<int> future = iface.future();
    QFutureWatcher<int> watcher;

    watcher.setFuture(future);
    QTest::qWait(10);

    iface.reportFinished();
    QVERIFY(future.isFinished());
    QVERIFY(watcher.isFinished() == false);

    QTest::qWait(10);
    QVERIFY(watcher.isFinished());
}

/*
    Verify that throttling kicks in if you report a lot of results,
    and that it clears when the result events are processed.
*/
void tst_QFutureWatcher::throttling()
{
    QFutureInterface<int> iface;
    iface.reportStarted();
    QFuture<int> future = iface.future();
    QFutureWatcher<int> watcher;
    watcher.setFuture(future);

    QVERIFY(iface.isThrottled() == false);

    for (int i = 0; i < 1000; ++i) {
        int result = 0;
        iface.reportResult(result);
    }

    QVERIFY(iface.isThrottled() == true);

    QTest::qWait(100); // process events.

    QVERIFY(iface.isThrottled() == false);

    iface.reportFinished();
}

int mapper(const int &i)
{
    return i;
}

class ResultReadyTester : public QObject
{
Q_OBJECT
public:
    ResultReadyTester(QFutureWatcher<int> *watcher)
    :m_watcher(watcher), filter(false), ok(true), count(0)
    {
        
    }
public slots:
    void resultReadyAt(int index)
    {
        ++count;
        if (m_watcher->future().isResultReadyAt(index) == false)
            ok = false;
        if (!filter && m_watcher->future().resultAt(index) != index)
            ok = false;
        if (filter && m_watcher->future().resultAt(index) != index * 2 + 1)
            ok = false;
    }
public:
    QFutureWatcher<int> *m_watcher;
    bool filter;
    bool ok;
    int count;
};

void tst_QFutureWatcher::incrementalMapResults()
{
    QFutureWatcher<int> watcher;

    SignalSlotObject object;
#ifdef PRINT
    connect(&watcher, SIGNAL(finished()), &object, SLOT(finished()));
    connect(&watcher, SIGNAL(progressValueChanged(int)), &object, SLOT(progressValueChanged(int)));
    connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
#endif

    QObject::connect(&watcher, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));

    ResultReadyTester resultReadyTester(&watcher);
    connect(&watcher, SIGNAL(resultReadyAt(int)), &resultReadyTester, SLOT(resultReadyAt(int)));

    const int count = 10000;
    QList<int> ints; 
    for (int i = 0; i < count; ++i)
        ints << i;

    QFuture<int> future = QtConcurrent::mapped(ints, mapper);
    watcher.setFuture(future);

    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QCOMPARE(resultReadyTester.count, count);
    QVERIFY(resultReadyTester.ok);
    QVERIFY(watcher.isFinished());
    future.waitForFinished(); 
}

bool filterer(int i)
{
    return (i % 2);
}

void tst_QFutureWatcher::incrementalFilterResults()
{
    QFutureWatcher<int> watcher;

    SignalSlotObject object;
#ifdef PRINT
    connect(&watcher, SIGNAL(finished()), &object, SLOT(finished()));
    connect(&watcher, SIGNAL(progressValueChanged(int)), &object, SLOT(progressValueChanged(int)));
    connect(&watcher, SIGNAL(resultReadyAt(int)), &object, SLOT(resultReadyAt(int)));
#endif

    QObject::connect(&watcher, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));


    ResultReadyTester resultReadyTester(&watcher);
    resultReadyTester.filter = true;
    connect(&watcher, SIGNAL(resultReadyAt(int)), &resultReadyTester, SLOT(resultReadyAt(int)));

    const int count = 10000;
    QList<int> ints; 
    for (int i = 0; i < count; ++i)
        ints << i;

    QFuture<int> future = QtConcurrent::filtered(ints, filterer);
    watcher.setFuture(future);

    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QCOMPARE(resultReadyTester.count, count / 2);
    QVERIFY(resultReadyTester.ok);
    QVERIFY(watcher.isFinished());
    future.waitForFinished(); 
}

void tst_QFutureWatcher::qfutureSynchornizer()
{
    int taskCount = 1000; 
    QTime t;
    t.start();

    {
        QFutureSynchronizer<void> sync;

        sync.setCancelOnWait(true);
        for (int i = 0; i < taskCount; ++i) {
            sync.addFuture(run(sleeper));
        }
    }

    // Test that we're not running each task.
    QVERIFY(t.elapsed() < taskCount * 10);
}

#include "tst_qfuturewatcher.moc"

#else
QTEST_NOOP_MAIN
#endif
