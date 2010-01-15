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


#include <QtCore/QtCore>
#include <QtTest/QtTest>

class tst_QCoreApplication: public QObject
{
    Q_OBJECT
private slots:
    void sendEventsOnProcessEvents(); // this must be the first test
    void getSetCheck();
    void qAppName();
    void argc();
    void postEvent();
    void removePostedEvents();
#ifndef QT_NO_THREAD
    void deliverInDefinedOrder();
#endif
    void applicationPid();
    void globalPostedEventsCount();
    void processEventsAlwaysSendsPostedEvents();
};

class EventSpy : public QObject
{
   Q_OBJECT

public:
    QList<int> recordedEvents;
    bool eventFilter(QObject *, QEvent *event)
    {
        recordedEvents.append(event->type());
        return false;
    }
};

void tst_QCoreApplication::sendEventsOnProcessEvents()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);

    EventSpy spy;
    app.installEventFilter(&spy);

    QCoreApplication::postEvent(&app,  new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::processEvents();
    QVERIFY(spy.recordedEvents.contains(QEvent::User + 1));
}

void tst_QCoreApplication::getSetCheck()
{
    // do not crash
    QString v = QCoreApplication::applicationVersion();
    v = QLatin1String("3.0.0 prerelease 1");
    QCoreApplication::setApplicationVersion(v);
    QCOMPARE(QCoreApplication::applicationVersion(), v);

    // Test the property
    {
        int argc = 1;
        char *argv[] = { "tst_qcoreapplication" };
        QCoreApplication app(argc, argv);
        QCOMPARE(app.property("applicationVersion").toString(), v);
    }
    v = QString();
    QCoreApplication::setApplicationVersion(v);
    QCOMPARE(QCoreApplication::applicationVersion(), v);
}

void tst_QCoreApplication::qAppName()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);
    QVERIFY(!::qAppName().isEmpty());
}

void tst_QCoreApplication::argc()
{
    {
        int argc = 1;
        char *argv[] = { "tst_qcoreapplication" };
        QCoreApplication app(argc, argv);
        QCOMPARE(argc, 1);
        QCOMPARE(app.argc(), 1);
    }

    {
        int argc = 4;
        char *argv[] = { "tst_qcoreapplication", "arg1", "arg2", "arg3" };
        QCoreApplication app(argc, argv);
        QCOMPARE(argc, 4);
        QCOMPARE(app.argc(), 4);
    }

    {
        int argc = 0;
        char **argv = 0;
        QCoreApplication app(argc, argv);
        QCOMPARE(argc, 0);
        QCOMPARE(app.argc(), 0);
    }
}

class EventGenerator : public QObject
{
    Q_OBJECT

public:
    QObject *other;

    bool event(QEvent *e)
    {
        if (e->type() == QEvent::MaxUser) {
            QCoreApplication::sendPostedEvents(other, 0);
        } else if (e->type() <= QEvent::User + 999) {
            // post a new event in response to this posted event
            int offset = e->type() - QEvent::User;
            offset = (offset * 10 + offset % 10);
            QCoreApplication::postEvent(this, new QEvent(QEvent::Type(QEvent::User + offset)), offset);
        }

        return QObject::event(e);
    }
};

void tst_QCoreApplication::postEvent()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);

    EventSpy spy;
    EventGenerator odd, even;
    odd.other = &even;
    odd.installEventFilter(&spy);
    even.other = &odd;
    even.installEventFilter(&spy);

    QCoreApplication::postEvent(&odd,  new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::postEvent(&even, new QEvent(QEvent::Type(QEvent::User + 2)));

    QCoreApplication::postEvent(&odd,  new QEvent(QEvent::Type(QEvent::User + 3)), 1);
    QCoreApplication::postEvent(&even, new QEvent(QEvent::Type(QEvent::User + 4)), 2);

    QCoreApplication::postEvent(&odd,  new QEvent(QEvent::Type(QEvent::User + 5)), -2);
    QCoreApplication::postEvent(&even, new QEvent(QEvent::Type(QEvent::User + 6)), -1);

    QList<int> expected;
    expected << QEvent::User + 4
             << QEvent::User + 3
             << QEvent::User + 1
             << QEvent::User + 2
             << QEvent::User + 6
             << QEvent::User + 5;

    QCoreApplication::sendPostedEvents();
    // live lock protection ensures that we only send the initial events
    QCOMPARE(spy.recordedEvents, expected);

    expected.clear();
    expected << QEvent::User + 66
             << QEvent::User + 55
             << QEvent::User + 44
             << QEvent::User + 33
             << QEvent::User + 22
             << QEvent::User + 11;

    spy.recordedEvents.clear();
    QCoreApplication::sendPostedEvents();
    // expect next sequence events
    QCOMPARE(spy.recordedEvents, expected);

    // have the generators call sendPostedEvents() on each other in
    // response to an event
    QCoreApplication::postEvent(&odd, new QEvent(QEvent::MaxUser), INT_MAX);
    QCoreApplication::postEvent(&even, new QEvent(QEvent::MaxUser), INT_MAX);

    expected.clear();
    expected << int(QEvent::MaxUser)
             << int(QEvent::MaxUser)
             << QEvent::User + 555
             << QEvent::User + 333
             << QEvent::User + 111
             << QEvent::User + 666
             << QEvent::User + 444
             << QEvent::User + 222;

    spy.recordedEvents.clear();
    QCoreApplication::sendPostedEvents();
    QCOMPARE(spy.recordedEvents, expected);

    expected.clear();
    expected << QEvent::User + 6666
             << QEvent::User + 5555
             << QEvent::User + 4444
             << QEvent::User + 3333
             << QEvent::User + 2222
             << QEvent::User + 1111;

    spy.recordedEvents.clear();
    QCoreApplication::sendPostedEvents();
    QCOMPARE(spy.recordedEvents, expected);

    // no more events
    expected.clear();
    spy.recordedEvents.clear();
    QCoreApplication::sendPostedEvents();
    QCOMPARE(spy.recordedEvents, expected);
}

void tst_QCoreApplication::removePostedEvents()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);

    EventSpy spy;
    QObject one, two;
    one.installEventFilter(&spy);
    two.installEventFilter(&spy);

    QList<int> expected;

    // remove all events for one object
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 1)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 2)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 3)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 4)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 5)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 6)));
    QCoreApplication::removePostedEvents(&one);
    expected << QEvent::User + 4
             << QEvent::User + 5
             << QEvent::User + 6;
    QCoreApplication::sendPostedEvents();
    QCOMPARE(spy.recordedEvents, expected);
    spy.recordedEvents.clear();
    expected.clear();

    // remove all events for all objects
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 7)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 8)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 9)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 10)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 11)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 12)));
    QCoreApplication::removePostedEvents(0);
    QCoreApplication::sendPostedEvents();
    QVERIFY(spy.recordedEvents.isEmpty());

    // remove a specific type of event for one object
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 13)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 14)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 15)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 16)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 17)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 18)));
    QCoreApplication::removePostedEvents(&one, QEvent::User + 13);
    QCoreApplication::removePostedEvents(&two, QEvent::User + 18);
    QCoreApplication::sendPostedEvents();
    expected << QEvent::User + 14
             << QEvent::User + 15
             << QEvent::User + 16
             << QEvent::User + 17;
    QCOMPARE(spy.recordedEvents, expected);
    spy.recordedEvents.clear();
    expected.clear();

    // remove a specific type of event for all objects
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 19)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 19)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 20)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 20)));
    QCoreApplication::postEvent(&one, new QEvent(QEvent::Type(QEvent::User + 21)));
    QCoreApplication::postEvent(&two, new QEvent(QEvent::Type(QEvent::User + 21)));
    QCoreApplication::removePostedEvents(0, QEvent::User + 20);
    QCoreApplication::sendPostedEvents();
    expected << QEvent::User + 19
             << QEvent::User + 19
             << QEvent::User + 21
             << QEvent::User + 21;
    QCOMPARE(spy.recordedEvents, expected);
    spy.recordedEvents.clear();
    expected.clear();
}

#ifndef QT_NO_THREAD
class DeliverInDefinedOrderThread : public QThread
{
    Q_OBJECT

public:
    DeliverInDefinedOrderThread()
        : QThread()
    { }

signals:
    void progress(int);

protected:
    void run()
    {
        emit progress(1);
        emit progress(2);
        emit progress(3);
        emit progress(4);
        emit progress(5);
        emit progress(6);
        emit progress(7);
    }
};

class DeliverInDefinedOrderObject : public QObject
{
    Q_OBJECT

    QPointer<QThread> thread;
    int count;

public:
    DeliverInDefinedOrderObject(QObject *parent)
        : QObject(parent), thread(0), count(0)
    { }
    ~DeliverInDefinedOrderObject()
    {
        if (!thread.isNull())
            thread->wait();
    }

public slots:
    void start()
    {
        QVERIFY(!thread);
        thread = new DeliverInDefinedOrderThread();
        connect(thread, SIGNAL(progress(int)), this, SLOT(threadProgress(int)));
        connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(thread, SIGNAL(destroyed()), this, SLOT(start()));
        thread->start();

        QCoreApplication::postEvent(this, new QEvent(QEvent::MaxUser), -1);
    }

    void threadProgress(int v)
    {
        ++count;
        QVERIFY(v == count);

        QCoreApplication::postEvent(this, new QEvent(QEvent::MaxUser), -1);
    }

    void threadFinished()
    {
        QVERIFY(count == 7);
        count = 0;

        QCoreApplication::postEvent(this, new QEvent(QEvent::MaxUser), -1);
    }

public:
    bool event(QEvent *event)
    {
        switch (event->type()) {
        case QEvent::User:
            {
                (void) QEventLoop().exec();
                break;
            }
        case QEvent::User + 1:
            break;
        default:
            break;
        }
        return QObject::event(event);
    }
};

void tst_QCoreApplication::deliverInDefinedOrder()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);

    DeliverInDefinedOrderObject obj(&app);
    // causes sendPostedEvents() to recurse twice
    QCoreApplication::postEvent(&obj, new QEvent(QEvent::User));
    QCoreApplication::postEvent(&obj, new QEvent(QEvent::User));
    // starts a thread that emits (queued) signals, which should be handled in order
    obj.start();

    // run for 15 seconds
    QTimer::singleShot(15000, &app, SLOT(quit()));
    app.exec();
}
#endif // QT_NO_QTHREAD

void tst_QCoreApplication::applicationPid()
{
    QVERIFY(QCoreApplication::applicationPid() > 0);
}

QT_BEGIN_NAMESPACE
Q_CORE_EXPORT uint qGlobalPostedEventsCount();
QT_END_NAMESPACE

class GlobalPostedEventsCountObject : public QObject
{
    Q_OBJECT

public:
    QList<int> globalPostedEventsCount;

    bool event(QEvent *event)
    {
        if (event->type() == QEvent::User)
            globalPostedEventsCount.append(qGlobalPostedEventsCount());
        return QObject::event(event);
    }
};

void tst_QCoreApplication::globalPostedEventsCount()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);

    QCoreApplication::sendPostedEvents();
    QCOMPARE(qGlobalPostedEventsCount(), 0u);

    GlobalPostedEventsCountObject x;
    QCoreApplication::postEvent(&x, new QEvent(QEvent::User));
    QCoreApplication::postEvent(&x, new QEvent(QEvent::User));
    QCoreApplication::postEvent(&x, new QEvent(QEvent::User));
    QCoreApplication::postEvent(&x, new QEvent(QEvent::User));
    QCoreApplication::postEvent(&x, new QEvent(QEvent::User));
    QCOMPARE(qGlobalPostedEventsCount(), 5u);

    QCoreApplication::sendPostedEvents();
    QCOMPARE(qGlobalPostedEventsCount(), 0u);

    QList<int> expected = QList<int>()
                          << 4
                          << 3
                          << 2
                          << 1
                          << 0;
    QCOMPARE(x.globalPostedEventsCount, expected);
}

class ProcessEventsAlwaysSendsPostedEventsObject : public QObject
{
public:
    int counter;

    inline ProcessEventsAlwaysSendsPostedEventsObject()
        : counter(0)
    { }

    bool event(QEvent *event)
    {
        if (event->type() == QEvent::User)
            ++counter;
        return QObject::event(event);
    }
};

void tst_QCoreApplication::processEventsAlwaysSendsPostedEvents()
{
    int argc = 1;
    char *argv[] = { "tst_qcoreapplication" };
    QCoreApplication app(argc, argv);

    ProcessEventsAlwaysSendsPostedEventsObject object;
    QTime t;
    t.start();
    int i = 1;
    do {
        QCoreApplication::postEvent(&object, new QEvent(QEvent::User));
        QCoreApplication::processEvents();
        QCOMPARE(object.counter, i);
        ++i;
    } while (t.elapsed() < 3000);
}

QTEST_APPLESS_MAIN(tst_QCoreApplication)
#include "tst_qcoreapplication.moc"
