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

#include <QDial>

class tst_QDial : public QObject
{
    Q_OBJECT
public:
    tst_QDial();

private slots:
    void getSetCheck();
    void valueChanged();
    void sliderMoved();
};

// Testing get/set functions
void tst_QDial::getSetCheck()
{
    QDial obj1;
    // bool QDial::notchesVisible()
    // void QDial::setNotchesVisible(bool)
    obj1.setNotchesVisible(false);
    QCOMPARE(false, obj1.notchesVisible());
    obj1.setNotchesVisible(true);
    QCOMPARE(true, obj1.notchesVisible());

    // bool QDial::wrapping()
    // void QDial::setWrapping(bool)
    obj1.setWrapping(false);
    QCOMPARE(false, obj1.wrapping());
    obj1.setWrapping(true);
    QCOMPARE(true, obj1.wrapping());
}

tst_QDial::tst_QDial()
{
}

void tst_QDial::valueChanged()
{
    QDial dial;
    dial.setMinimum(0);
    dial.setMaximum(100);
    QSignalSpy spy(&dial, SIGNAL(valueChanged(int)));
    dial.setValue(50);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    dial.setValue(25);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    // repeat!
    dial.setValue(25);
    QCOMPARE(spy.count(), 0);
}

void tst_QDial::sliderMoved()
{
    //this tests that when dragging the arrow that the sliderMoved signal is emitted
    //even if tracking is set to false
    QDial dial;
    dial.setTracking(false);
    dial.setMinimum(0);
    dial.setMaximum(100);

    dial.show();

    QPoint init(dial.width()/4, dial.height()/2);

    QMouseEvent pressevent(QEvent::MouseButtonPress, init,
        Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(&dial, &pressevent);

    QSignalSpy sliderspy(&dial, SIGNAL(sliderMoved(int)));
    QSignalSpy valuespy(&dial, SIGNAL(valueChanged(int)));


    { //move on top of the slider
        init = QPoint(dial.width()/2, dial.height()/4);
        QMouseEvent moveevent(QEvent::MouseMove, init,
            Qt::LeftButton, Qt::LeftButton, 0);
        qApp->sendEvent(&dial, &moveevent);
        QCOMPARE( sliderspy.count(), 1);
        QCOMPARE( valuespy.count(), 0);
    }


    { //move on the right of the slider
        init = QPoint(dial.width()*3/4, dial.height()/2);
        QMouseEvent moveevent(QEvent::MouseMove, init,
            Qt::LeftButton, Qt::LeftButton, 0);
        qApp->sendEvent(&dial, &moveevent);
        QCOMPARE( sliderspy.count(), 2);
        QCOMPARE( valuespy.count(), 0);
    }

    QMouseEvent releaseevent(QEvent::MouseButtonRelease, init,
        Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(&dial, &releaseevent);
    QCOMPARE( valuespy.count(), 1); // valuechanged signal should be called at this point

}

QTEST_MAIN(tst_QDial)
#include "tst_qdial.moc"
