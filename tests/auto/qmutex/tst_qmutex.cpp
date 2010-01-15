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


#include <QtTest/QtTest>

#include <qatomic.h>
#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qmutex.h>
#include <qthread.h>
#include <qwaitcondition.h>



//TESTED_CLASS=
//TESTED_FILES=

class tst_QMutex : public QObject
{
    Q_OBJECT

public:
    tst_QMutex();
    virtual ~tst_QMutex();

private slots:
    void tryLock();
    void lock_unlock_locked_tryLock();
    void stressTest();
    void tryLockRace();
};

static const int iterations = 100;

tst_QMutex::tst_QMutex()

{
}

tst_QMutex::~tst_QMutex()
{
}

QAtomicInt lockCount(0);
QMutex normalMutex, recursiveMutex(QMutex::Recursive);
QSemaphore testsTurn;
QSemaphore threadsTurn;

void tst_QMutex::tryLock()
{
    // test non-recursive mutex
    {
        class Thread : public QThread
        {
        public:
            void run()
            {
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(!normalMutex.tryLock());
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(normalMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(!normalMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                normalMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
                QTime timer;
                timer.start();
                QVERIFY(!normalMutex.tryLock(1000));
                QVERIFY(timer.elapsed() >= 1000);
                testsTurn.release();

                threadsTurn.acquire();
                timer.start();
                QVERIFY(normalMutex.tryLock(1000));
                QVERIFY(timer.elapsed() <= 1000);
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                timer.start();
                QVERIFY(!normalMutex.tryLock(1000));
                QVERIFY(timer.elapsed() >= 1000);
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                normalMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
            }
        };

        Thread thread;
        thread.start();

        testsTurn.acquire();
        normalMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        threadsTurn.release();

        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        normalMutex.unlock();
        threadsTurn.release();

        testsTurn.acquire();
        normalMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        threadsTurn.release();

        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        normalMutex.unlock();
        threadsTurn.release();

        // wait for thread to finish
        testsTurn.acquire();
        threadsTurn.release();
        thread.wait();
    }

    // test recursive mutex
    {
        class Thread : public QThread
        {
        public:
            void run()
            {
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(!recursiveMutex.tryLock());
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(recursiveMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(recursiveMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(1, 2));
                QVERIFY(lockCount.testAndSetRelaxed(2, 1));
                recursiveMutex.unlock();
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                recursiveMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
                QTime timer;
                timer.start();
                QVERIFY(!recursiveMutex.tryLock(1000));
                QVERIFY(timer.elapsed() >= 1000);
                testsTurn.release();

                threadsTurn.acquire();
                timer.start();
                QVERIFY(recursiveMutex.tryLock(1000));
                QVERIFY(timer.elapsed() <= 1000);
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(recursiveMutex.tryLock(1000));
                QVERIFY(lockCount.testAndSetRelaxed(1, 2));
                QVERIFY(lockCount.testAndSetRelaxed(2, 1));
                recursiveMutex.unlock();
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                recursiveMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
            }
        };

        Thread thread;
        thread.start();

        testsTurn.acquire();
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 2));
        threadsTurn.release();

        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(2, 1));
        recursiveMutex.unlock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        recursiveMutex.unlock();
        threadsTurn.release();

        testsTurn.acquire();
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 2));
        threadsTurn.release();

        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(2, 1));
        recursiveMutex.unlock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        recursiveMutex.unlock();
        threadsTurn.release();

        // stop thread
        testsTurn.acquire();
        threadsTurn.release();
        thread.wait();
    }
}

class mutex_Thread : public QThread
{
public:
    QMutex mutex;
    QWaitCondition cond;

    QMutex &test_mutex;

    inline mutex_Thread(QMutex &m) : test_mutex(m) { }

    void run()
    {
	test_mutex.lock();

	mutex.lock();
	for (int i = 0; i < iterations; ++i) {
	    cond.wakeOne();
	    cond.wait(&mutex);
	}
	mutex.unlock();

    	test_mutex.unlock();
    }
};

class rmutex_Thread : public QThread
{
public:
    QMutex mutex;
    QWaitCondition cond;

    QMutex &test_mutex;

    inline rmutex_Thread(QMutex &m) : test_mutex(m) { }

    void run()
    {
	test_mutex.lock();
	test_mutex.lock();
	test_mutex.lock();
	test_mutex.lock();

	mutex.lock();
	for (int i = 0; i < iterations; ++i) {
	    cond.wakeOne();
	    cond.wait(&mutex);
	}
	mutex.unlock();

    	test_mutex.unlock();
    	test_mutex.unlock();
    	test_mutex.unlock();
    	test_mutex.unlock();
    }
};

#ifdef QT3_SUPPORT
#define VERIFY_LOCKED(x) QVERIFY((x).locked())
#define VERIFY_NLOCKED(x) QVERIFY(!(x).locked())
#else
#define VERIFY_LOCKED(x)
#define VERIFY_NLOCKED(x)
#endif // QT3_SUPPORT

void tst_QMutex::lock_unlock_locked_tryLock()
{
    // normal mutex
    QMutex mutex;
    mutex_Thread thread(mutex);

    QMutex rmutex(QMutex::Recursive);
    rmutex_Thread rthread(rmutex);

    for (int i = 0; i < iterations; ++i) {
	// normal mutex
	VERIFY_NLOCKED(mutex);
	QVERIFY(mutex.tryLock());
	mutex.unlock();

	thread.mutex.lock();
	thread.start();

	for (int j = 0; j < iterations; ++j) {
	    QVERIFY(thread.cond.wait(&thread.mutex, 10000));
	    VERIFY_LOCKED(mutex);
	    QVERIFY(!mutex.tryLock());

	    thread.cond.wakeOne();
	}

	thread.mutex.unlock();

	QVERIFY(thread.wait(10000));
        VERIFY_NLOCKED(mutex);
	QVERIFY(mutex.tryLock());

	mutex.unlock();

    	// recursive mutex
        VERIFY_NLOCKED(rmutex);
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());

	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();

	rthread.mutex.lock();
	rthread.start();

	for (int k = 0; k < iterations; ++k) {
	    QVERIFY(rthread.cond.wait(&rthread.mutex, 10000));
            VERIFY_LOCKED(rmutex);
	    QVERIFY(!rmutex.tryLock());

	    rthread.cond.wakeOne();
	}

	rthread.mutex.unlock();

	QVERIFY(rthread.wait(10000));
        VERIFY_NLOCKED(rmutex);
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());

	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();
    }
}

enum { one_minute = 60 * 1000, threadCount = 10 };

class StressTestThread : public QThread
{
    QTime t;
public:
    static QBasicAtomicInt lockCount;
    static QBasicAtomicInt sentinel;
    static QMutex mutex;
    void start()
    {
        t.start();
        QThread::start();
    }
    void run()
    {
        while (t.elapsed() < one_minute) {
            mutex.lock();
            Q_ASSERT(!sentinel.ref());
            Q_ASSERT(sentinel.deref());
            lockCount.ref();
            mutex.unlock();
            if (mutex.tryLock()) {
                Q_ASSERT(!sentinel.ref());
                Q_ASSERT(sentinel.deref());
                lockCount.ref();
                mutex.unlock();
            }
        }
    }
};
QMutex StressTestThread::mutex;
QBasicAtomicInt StressTestThread::lockCount = Q_BASIC_ATOMIC_INITIALIZER(0);
QBasicAtomicInt StressTestThread::sentinel = Q_BASIC_ATOMIC_INITIALIZER(-1);

void tst_QMutex::stressTest()
{
    StressTestThread threads[threadCount];
    for (int i = 0; i < threadCount; ++i)
        threads[i].start();
    QVERIFY(threads[0].wait(one_minute + 10000));
    for (int i = 1; i < threadCount; ++i)
        QVERIFY(threads[i].wait(10000));
    qDebug("locked %d times", int(StressTestThread::lockCount));
}

class TryLockRaceThread : public QThread
{
public:
    static QMutex mutex;

    void run()
    {
        QTime t;
        t.start();
        do {
            if (mutex.tryLock())
                mutex.unlock();
        } while (t.elapsed() < 20000);
    }
};
QMutex TryLockRaceThread::mutex;

void tst_QMutex::tryLockRace()
{
    // mutex not in use, should be able to lock it
    QVERIFY(TryLockRaceThread::mutex.tryLock());
    TryLockRaceThread::mutex.unlock();

    // try to break tryLock
    TryLockRaceThread thread[threadCount];
    for (int i = 0; i < threadCount; ++i)
        thread[i].start();
    for (int i = 0; i < threadCount; ++i)
        QVERIFY(thread[i].wait());

    // mutex not in use, should be able to lock it
    QVERIFY(TryLockRaceThread::mutex.tryLock());
    TryLockRaceThread::mutex.unlock();
}

QTEST_MAIN(tst_QMutex)
#include "tst_qmutex.moc"
