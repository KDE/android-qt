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
#include <QtCore/QtDebug>
#include <QtTest/QtTest>

class tst_QDebug: public QObject
{
    Q_OBJECT
private slots:
    void assignment() const;
    void warningWithoutDebug() const;
    void criticalWithoutDebug() const;
    void debugWithQBool() const;
    void veryLongWarningMessage() const;
    void qDebugQStringRef() const;
};

void tst_QDebug::assignment() const
{
    QDebug debug1(QtDebugMsg);
    QDebug debug2(QtWarningMsg);

    QTest::ignoreMessage(QtDebugMsg, "foo ");
    QTest::ignoreMessage(QtWarningMsg, "bar 1 2 ");

    debug1 << "foo";
    debug2 << "bar";
    debug1 = debug2;
    debug1 << "1";
    debug2 << "2";
}

static QtMsgType s_msgType;
static QByteArray s_msg;

static void myMessageHandler(QtMsgType type, const char *msg)
{
    s_msg = msg;
    s_msgType = type;
}

/*! \internal
  The qWarning() stream should be usable even if QT_NO_DEBUG is defined.
 */
void tst_QDebug::warningWithoutDebug() const
{
    qInstallMsgHandler(myMessageHandler);
    { qWarning() << "A qWarning() message"; }
    QCOMPARE(s_msgType, QtWarningMsg);
    QCOMPARE(QString::fromLatin1(s_msg.data()), QString::fromLatin1("A qWarning() message "));
    qInstallMsgHandler(0);
}

/*! \internal
  The qCritical() stream should be usable even if QT_NO_DEBUG is defined.
 */
void tst_QDebug::criticalWithoutDebug() const
{
    qInstallMsgHandler(myMessageHandler);
    { qCritical() << "A qCritical() message"; }
    QCOMPARE(s_msgType, QtCriticalMsg);
    QCOMPARE(QString::fromLatin1(s_msg), QString::fromLatin1("A qCritical() message "));
    qInstallMsgHandler(0);
}

void tst_QDebug::debugWithQBool() const
{
    qInstallMsgHandler(myMessageHandler);
    { qDebug() << QBool(false) << QBool(true); }
    QCOMPARE(s_msgType, QtDebugMsg);
    QCOMPARE(QString::fromLatin1(s_msg), QString::fromLatin1("false true "));
    qInstallMsgHandler(0);
}

void tst_QDebug::veryLongWarningMessage() const
{
    qInstallMsgHandler(myMessageHandler);
    QString test;
    {
        QString part("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\n");
        for (int i = 0; i < 1000; ++i)
            test.append(part);
        qWarning("Test output:\n%s\nend", qPrintable(test));
    }
    QCOMPARE(s_msgType, QtWarningMsg);
    QCOMPARE(QString::fromLatin1(s_msg), QString::fromLatin1("Test output:\n")+test+QString::fromLatin1("\nend"));
    qInstallMsgHandler(0);
}

void tst_QDebug::qDebugQStringRef() const
{
    /* Use a basic string. */
    {
        const QString in(QLatin1String("input"));
        const QStringRef inRef(&in);

        qInstallMsgHandler(myMessageHandler);
        { qDebug() << inRef; }
        QCOMPARE(s_msgType, QtDebugMsg);
        QCOMPARE(QString::fromLatin1(s_msg), QString::fromLatin1("\"input\" "));
        qInstallMsgHandler(0);
    }

    /* Use a null QStringRef. */
    {
        const QStringRef inRef;

        qInstallMsgHandler(myMessageHandler);
        { qDebug() << inRef; }
        QCOMPARE(s_msgType, QtDebugMsg);
        QCOMPARE(QString::fromLatin1(s_msg), QString::fromLatin1("\"\" "));
        qInstallMsgHandler(0);
    }
}

QTEST_MAIN(tst_QDebug);
#include "tst_qdebug.moc"
