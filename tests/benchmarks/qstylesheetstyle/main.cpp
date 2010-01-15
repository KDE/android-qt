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
// This file contains benchmarks for QRect/QRectF functions.

#include <QtGui>
#include <qtest.h>

class tst_qstylesheetstyle : public QObject
{
    Q_OBJECT
private slots:
    void empty();
    void empty_events();

    void simple();
    void simple_events();

    void grid_data();
    void grid();

private:
    QWidget *buildSimpleWidgets();

};


QWidget *tst_qstylesheetstyle::buildSimpleWidgets()
{
    QWidget *w = new QWidget();
    QGridLayout *layout = new QGridLayout(w);
    w->setLayout(layout);
    layout->addWidget(new QPushButton("pushButton")     ,0,0);
    layout->addWidget(new QComboBox()                   ,0,1);
    layout->addWidget(new QCheckBox("checkBox")         ,0,2);
    layout->addWidget(new QRadioButton("radioButton")   ,0,3);
    layout->addWidget(new QLineEdit()                   ,1,0);
    layout->addWidget(new QLabel("label")               ,1,1);
    layout->addWidget(new QSpinBox()                    ,1,2);
    layout->addWidget(new QProgressBar()                ,1,3);
    return w;
}

void tst_qstylesheetstyle::empty()
{
    QWidget *w = buildSimpleWidgets();
    w->setStyleSheet("/* */");
    int i = 0;
    QBENCHMARK {
        w->setStyleSheet("/*" + QString::number(i) + "*/");
        i++; // we want a different string in case we have severals iterations
    }
    delete w;
}

void tst_qstylesheetstyle::empty_events()
{
    QWidget *w = buildSimpleWidgets();
    w->setStyleSheet("/* */");
    int i = 0;
    QBENCHMARK {
        w->setStyleSheet("/*" + QString::number(i) + "*/");
        i++; // we want a different string in case we have severals iterations
        qApp->processEvents();
    }
    delete w;
}

static const char *simple_css =
   " QLineEdit { background: red; }   QPushButton { border: 1px solid yellow; color: pink; }  \n"
   " QCheckBox { margin: 3px 5px; background-color:red; } QAbstractButton { background-color: #456; } \n"
   " QFrame { padding: 3px; } QLabel { color: black } QSpinBox:hover { background-color:blue; }  ";

void tst_qstylesheetstyle::simple()
{
    QWidget *w = buildSimpleWidgets();
    w->setStyleSheet("/* */");
    int i = 0;
    QBENCHMARK {
        w->setStyleSheet(QString(simple_css) + "/*" + QString::number(i) + "*/");
        i++; // we want a different string in case we have severals iterations
    }
    delete w;
}

void tst_qstylesheetstyle::simple_events()
{
    QWidget *w = buildSimpleWidgets();
    w->setStyleSheet("/* */");
    int i = 0;
    QBENCHMARK {
        w->setStyleSheet(QString(simple_css) + "/*" + QString::number(i) + "*/");
        i++; // we want a different string in case we have severals iterations
        qApp->processEvents();
    }
    delete w;
}

void tst_qstylesheetstyle::grid_data()
{
        QTest::addColumn<bool>("events");
        QTest::addColumn<bool>("show");
        QTest::addColumn<int>("N");
        for (int n = 5; n <= 25; n += 5) {
           const QByteArray nString = QByteArray::number(n*n);
            QTest::newRow(("simple--" + nString).constData()) << false << false << n;
            QTest::newRow(("events--" + nString).constData()) << true << false << n;
            QTest::newRow(("show--" + nString).constData()) << true << true << n;
        }
}


void tst_qstylesheetstyle::grid()
{
    QFETCH(bool, events);
    QFETCH(bool, show);
    QFETCH(int, N);

#ifdef Q_OS_SYMBIAN
    // Symbian has limited stack (max 80k), which will run out when N >= 20 due to
    // QWidget::show() using recursion among grid labels somewhere down the line.
    if (show && N >= 20)
        QSKIP("Grid too big for device to show", SkipSingle);
#endif

    QWidget *w = new QWidget();
    QGridLayout *layout = new QGridLayout(w);
    w->setLayout(layout);
    QString stylesheet;
    for(int x=0; x<N ;x++)
        for(int y=0; y<N ;y++) {
        QLabel *label = new QLabel(QString::number(y * N + x));
        layout->addWidget(label ,x,y);
        label->setObjectName(QString("label%1").arg(y * N + x));
        stylesheet += QString("#label%1 { background-color: rgb(0,%2,%3); color: rgb(%2,%3,255);  } ").arg(y*N+x).arg(y*255/N).arg(x*255/N);
    }

    w->setStyleSheet("/* */");
    if(show) {
        w->show();
        QTest::qWait(30);
    }
    int i = 0;
    QBENCHMARK {
        w->setStyleSheet(stylesheet + "/*" + QString::number(i) + "*/");
        i++; // we want a different string in case we have severals iterations
        if(events)
            qApp->processEvents();
    }
    delete w;
}

QTEST_MAIN(tst_qstylesheetstyle)

#include "main.moc"
