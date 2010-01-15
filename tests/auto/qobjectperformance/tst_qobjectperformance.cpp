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

#include <qcoreapplication.h>

#include <qobject.h>

//TESTED_CLASS=
//TESTED_FILES=corelib/kernel/qobject.h corelib/kernel/qobject.cpp

class tst_QObjectPerformance : public QObject
{
    Q_OBJECT

public:

private slots:
    void emitToManyReceivers();
};

class SimpleSenderObject : public QObject
{
    Q_OBJECT

signals:
    void signal();

public:
    void emitSignal()
    {
        emit signal();
    }
};

class SimpleReceiverObject : public QObject
{
    Q_OBJECT

public slots:
    void slot()
    {
    }
};

void tst_QObjectPerformance::emitToManyReceivers()
{
    // ensure that emission times remain mostly linear as the number of receivers increase

    SimpleSenderObject sender;
    int elapsed = 0;
    const int increase = 3000;
    const int base = 5000;

    for (int i = 0; i < 4; ++i) {
        const int size = base + i * increase;
        const double increaseRatio = double(size) / (double)(size - increase);

        QList<SimpleReceiverObject *> receivers;
        for (int k = 0; k < size; ++k) {
            SimpleReceiverObject *receiver = new SimpleReceiverObject;
            QObject::connect(&sender, SIGNAL(signal()), receiver, SLOT(slot()));
            receivers.append(receiver);
        }

        QTime timer;
        timer.start();
        sender.emitSignal();
        int e = timer.elapsed();

        if (elapsed > 1) {
            qDebug() << size << "receivers, elapsed time" << e << "compared to previous time" << elapsed;
            QVERIFY(double(e) / double(elapsed) <= increaseRatio * 2.0);
        } else {
            qDebug() << size << "receivers, elapsed time" << e << "cannot be compared to previous, unmeasurable time";
        }
        elapsed = e;

        qDeleteAll(receivers);
        receivers.clear();
    }
}


QTEST_MAIN(tst_QObjectPerformance)
#include "tst_qobjectperformance.moc"
