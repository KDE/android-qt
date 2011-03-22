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

/*
    This test makes a testlog containing lots of characters which have a special meaning in
    XML, with the purpose of exposing bugs in testlib's XML output code.
*/
class tst_BadXml : public QObject
{
    Q_OBJECT

private slots:
    void badDataTag() const;
    void badDataTag_data() const;

    void badMessage() const;
    void badMessage_data() const;

    void failWithNoFile() const;

public:
    static QList<QByteArray> const& badStrings();
};

/*
    Custom metaobject to make it possible to change class name at runtime.
*/
class EmptyClass : public tst_BadXml
{ Q_OBJECT };

class tst_BadXmlSub : public tst_BadXml
{
public:
    const QMetaObject* metaObject() const;

    static char const* className;
};
char const* tst_BadXmlSub::className = "tst_BadXml";

const QMetaObject* tst_BadXmlSub::metaObject() const
{
    const QMetaObject& empty = EmptyClass::staticMetaObject;
    static QMetaObject mo = {
        { empty.d.superdata, empty.d.stringdata, empty.d.data, empty.d.extradata }
    };
    static char currentClassName[1024];
    qstrcpy(currentClassName, className);
    int len = qstrlen(className);
    currentClassName[len] = 0;
    currentClassName[len+1] = 0;

    mo.d.stringdata = currentClassName;

    return &mo;
}

/*
    Outputs incidents and benchmark results with the current data tag set to a bad string.
*/
void tst_BadXml::badDataTag() const
{
    qDebug("a message");

    QBENCHMARK {
    }

    QFAIL("a failure");
}

void tst_BadXml::badDataTag_data() const
{
    QTest::addColumn<int>("dummy");

    foreach (char const* str, badStrings()) {
        QTest::newRow(str) << 0;
    }
}

void tst_BadXml::failWithNoFile() const
{
    QTest::qFail("failure message", 0, 0);
}

/*
    Outputs a message containing a bad string.
*/
void tst_BadXml::badMessage() const
{
    QFETCH(QByteArray, message);
    qDebug("%s", message.constData());
}

void tst_BadXml::badMessage_data() const
{
    QTest::addColumn<QByteArray>("message");

    int i = 0;
    foreach (QByteArray const& str, badStrings()) {
        QTest::newRow(qPrintable(QString::fromLatin1("string %1").arg(i++))) << str;
    }
}

/*
    Returns a list of strings likely to expose bugs in XML output code.
*/
QList<QByteArray> const& tst_BadXml::badStrings()
{
    static QList<QByteArray> out;
    if (out.isEmpty()) {
        out << "end cdata ]]> text ]]> more text";
        out << "quotes \" text\" more text";
        out << "xml close > open < tags < text";
        out << "all > \" mixed ]]> up > \" in < the ]]> hopes < of triggering \"< ]]> bugs";
    }
    return out;
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    /*
        tst_selftests can't handle multiple XML documents in a single testrun, so we'll
        decide before we begin which of our "bad strings" we want to use for our testcase
        name.
    */
    int badstring = -1;
    QVector<char const*> args;
    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-badstring")) {
            bool ok = false;
            if (i < argc-1) {
                badstring = QByteArray(argv[i+1]).toInt(&ok);
                ++i;
            }
            if (!ok) {
                qFatal("Bad `-badstring' option");
            }
        }
        else {
            args << argv[i];
        }
    }
    /*
        We just want testlib to output a benchmark result, we don't actually care about the value,
        so just do one iteration to save time.
    */
    args << "-iterations" << "1";

    if (badstring == -1) {
        tst_BadXml test;
        return QTest::qExec(&test, args.count(), const_cast<char**>(args.data()));
    }

    QList<QByteArray> badstrings = tst_BadXml::badStrings();
    if (badstring >= badstrings.count())
        qFatal("`-badstring %d' is out of range", badstring);

    tst_BadXmlSub test;
    test.className = badstrings[badstring].constData();
    return QTest::qExec(&test, args.count(), const_cast<char**>(args.data()));
}

#include "tst_badxml.moc"
