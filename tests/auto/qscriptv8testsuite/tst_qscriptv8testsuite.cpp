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


#include "abstracttestsuite.h"
#include <QtTest/QtTest>
#include <QtScript>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QScriptV8TestSuite : public AbstractTestSuite
{
public:
    tst_QScriptV8TestSuite();
    virtual ~tst_QScriptV8TestSuite();

protected:
    struct ExpectedFailure
    {
        ExpectedFailure(const QString &name, const QString &act,
                        const QString &exp, const QString &msg)
            : testName(name), actual(act), expected(exp), message(msg)
            { }

        QString testName;
        QString actual;
        QString expected;
        QString message;
    };

    void addExpectedFailure(const QString &testName, const QString &actual,
                            const QString &expected, const QString &message);
    bool isExpectedFailure(const QString &testName, const QString &actual,
                           const QString &expected, QString *message) const;
    void addTestExclusion(const QString &testName, const QString &message);
    void addTestExclusion(const QRegExp &rx, const QString &message);
    bool isExcludedTest(const QString &testName, QString *message) const;

    virtual void initTestCase();
    virtual void configData(TestConfig::Mode mode, const QStringList &parts);
    virtual void writeSkipConfigFile(QTextStream &);
    virtual void writeExpectFailConfigFile(QTextStream &);
    virtual void runTestFunction(int testIndex);

    QStringList testNames;
    QList<ExpectedFailure> expectedFailures;
    QList<QPair<QRegExp, QString> > testExclusions;
    QString mjsunitContents;
};

// We expect failing tests to call the fail() function (defined in
// mjsunit.js) with arguments expected, actual, message_opt.  This
// function intercepts the call, calls the real fail() function (which
// will throw an exception), and sets the original arguments on the
// exception object so that we can process them later.
static QScriptValue qscript_fail(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue realFail = ctx->callee().data();
    Q_ASSERT(realFail.isFunction());
    QScriptValue ret = realFail.call(ctx->thisObject(), ctx->argumentsObject());
    Q_ASSERT(eng->hasUncaughtException());
    ret.setProperty("expected", ctx->argument(0));
    ret.setProperty("actual", ctx->argument(1));
    ret.setProperty("message", ctx->argument(2));
    QScriptContextInfo info(ctx->parentContext()->parentContext());
    ret.setProperty("lineNumber", info.lineNumber());
    return ret;
}

void tst_QScriptV8TestSuite::writeSkipConfigFile(QTextStream &stream)
{
    stream << QString::fromLatin1("# testcase | message") << endl;
}

void tst_QScriptV8TestSuite::writeExpectFailConfigFile(QTextStream &stream)
{
    stream << QString::fromLatin1("# testcase | actual | expected | message") << endl;
    for (int i = 0; i < expectedFailures.size(); ++i) {
        const ExpectedFailure &fail = expectedFailures.at(i);
        stream << QString::fromLatin1("%0 | %1 | %2")
            .arg(fail.testName)
            .arg(escape(fail.actual))
            .arg(escape(fail.expected));
        if (!fail.message.isEmpty())
            stream << QString::fromLatin1(" | %0").arg(escape(fail.message));
        stream << endl;
    }
}

void tst_QScriptV8TestSuite::runTestFunction(int testIndex)
{
    QString name = testNames.at(testIndex);
    QString path = testsDir.absoluteFilePath(name + ".js");

    QString excludeMessage;
    if (isExcludedTest(name, &excludeMessage)) {
        QTest::qSkip(excludeMessage.toLatin1(), QTest::SkipAll, path.toLatin1(), -1);
        return;
    }

    QScriptEngine engine;
    engine.evaluate(mjsunitContents);
    if (engine.hasUncaughtException()) {
        QStringList bt = engine.uncaughtExceptionBacktrace();
        QString err = engine.uncaughtException().toString();
        qWarning("%s\n%s", qPrintable(err), qPrintable(bt.join("\n")));
    } else {
        // Prepare to intercept calls to mjsunit's fail() function.
        QScriptValue fakeFail = engine.newFunction(qscript_fail);
        fakeFail.setData(engine.globalObject().property("fail"));
        engine.globalObject().setProperty("fail", fakeFail);

        QString contents = readFile(path);
        QScriptValue ret = engine.evaluate(contents);
        if (engine.hasUncaughtException()) {
            if (!ret.isError()) {
                Q_ASSERT(ret.instanceOf(engine.globalObject().property("MjsUnitAssertionError")));
                QString actual = ret.property("actual").toString();
                QString expected = ret.property("expected").toString();
                int lineNumber = ret.property("lineNumber").toInt32();
                QString failMessage;
                if (shouldGenerateExpectedFailures) {
                    if (ret.property("message").isString())
                        failMessage = ret.property("message").toString();
                    addExpectedFailure(name, actual, expected, failMessage);
                } else if (isExpectedFailure(name, actual, expected, &failMessage)) {
                    QTest::qExpectFail("", failMessage.toLatin1(),
                                       QTest::Continue, path.toLatin1(),
                                       lineNumber);
                }
                QTest::qCompare(actual, expected, "actual", "expect",
                                path.toLatin1(), lineNumber);
            } else {
                int lineNumber = ret.property("lineNumber").toInt32();
                QTest::qExpectFail("", ret.toString().toLatin1(),
                                   QTest::Continue, path.toLatin1(), lineNumber);
                QTest::qVerify(false, ret.toString().toLatin1(), "", path.toLatin1(), lineNumber);
            }
        }
    }
}

tst_QScriptV8TestSuite::tst_QScriptV8TestSuite()
    : AbstractTestSuite("tst_QScriptV8TestSuite",
                        ":/tests", ":/")
{
    // One test function per test file.
    QFileInfoList testFileInfos;
    testFileInfos = testsDir.entryInfoList(QStringList() << "*.js", QDir::Files);
    foreach (QFileInfo tfi, testFileInfos) {
        QString name = tfi.baseName();
        addTestFunction(name);
        testNames.append(name);
    }

    finalizeMetaObject();
}

tst_QScriptV8TestSuite::~tst_QScriptV8TestSuite()
{
}

void tst_QScriptV8TestSuite::initTestCase()
{
    AbstractTestSuite::initTestCase();

    // FIXME: These warnings should be QFAIL, but that would make the
    // test fail right now.
    if (!testsDir.exists("mjsunit.js"))
        qWarning("*** no tests/mjsunit.js file!");
    else {
        mjsunitContents = readFile(testsDir.absoluteFilePath("mjsunit.js"));
        if (mjsunitContents.isEmpty())
            qWarning("*** tests/mjsunit.js is empty!");
    }
}

void tst_QScriptV8TestSuite::configData(TestConfig::Mode mode, const QStringList &parts)
{
    switch (mode) {
    case TestConfig::Skip:
        addTestExclusion(parts.at(0), parts.value(1));
        break;

    case TestConfig::ExpectFail:
        addExpectedFailure(parts.at(0), parts.value(1),
                           parts.value(2), parts.value(3));
        break;
    }
}

void tst_QScriptV8TestSuite::addExpectedFailure(const QString &testName, const QString &actual,
                                   const QString &expected, const QString &message)
{
    expectedFailures.append(ExpectedFailure(testName, actual, expected, message));
}

bool tst_QScriptV8TestSuite::isExpectedFailure(const QString &testName, const QString &actual,
                                  const QString &expected, QString *message) const
{
    for (int i = 0; i < expectedFailures.size(); ++i) {
        const ExpectedFailure &ef = expectedFailures.at(i);
        if ((testName == ef.testName) && (actual == ef.actual) && (expected == ef.expected)) {
            if (message)
                *message = ef.message;
            return true;
        }
    }
    return false;
}

void tst_QScriptV8TestSuite::addTestExclusion(const QString &testName, const QString &message)
{
    testExclusions.append(qMakePair(QRegExp(testName), message));
}

void tst_QScriptV8TestSuite::addTestExclusion(const QRegExp &rx, const QString &message)
{
    testExclusions.append(qMakePair(rx, message));
}

bool tst_QScriptV8TestSuite::isExcludedTest(const QString &testName, QString *message) const
{
    for (int i = 0; i < testExclusions.size(); ++i) {
        if (testExclusions.at(i).first.indexIn(testName) != -1) {
            if (message)
                *message = testExclusions.at(i).second;
            return true;
        }
    }
    return false;
}

QTEST_MAIN(tst_QScriptV8TestSuite)
