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

#ifndef ABSTRACTTESTSUITE_H
#define ABSTRACTTESTSUITE_H

#include <QtCore/qobject.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qdir.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qtextstream.h>

class TestMetaObjectBuilder;

namespace TestConfig {
enum Mode {
    Skip,
    ExpectFail
};
}

// For receiving callbacks from the config parser.
class TestConfigClientInterface
{
public:
    virtual ~TestConfigClientInterface() {}
    virtual void configData(TestConfig::Mode mode,
                            const QStringList &parts) = 0;
    virtual void configError(const QString &path,
                             const QString &message,
                             int lineNumber) = 0;
};

class AbstractTestSuite : public QObject,
                          public TestConfigClientInterface
{
// No Q_OBJECT macro, we implement the meta-object ourselves.
public:
    AbstractTestSuite(const QByteArray &className,
                      const QString &defaultTestsPath,
                      const QString &defaultConfigPath);
    virtual ~AbstractTestSuite();

    static QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

    static QString readFile(const QString &);
    static QString escape(const QString &);

protected:
    enum DataFunctionCreation {
        DontCreateDataFunction,
        CreateDataFunction
    };

    void addTestFunction(const QString &,
                         DataFunctionCreation = DontCreateDataFunction);
    void finalizeMetaObject();

    virtual void initTestCase();
    virtual void cleanupTestCase();

    virtual void writeSkipConfigFile(QTextStream &) = 0;
    virtual void writeExpectFailConfigFile(QTextStream &) = 0;

    virtual void runTestFunction(int index) = 0;

    virtual void configError(const QString &path, const QString &message, int lineNumber);

    QDir testsDir;
    bool shouldGenerateExpectedFailures;

private:
    TestMetaObjectBuilder *metaBuilder;
    QString skipConfigPath, expectFailConfigPath;

private:
    void createSkipConfigFile();
    void createExpectFailConfigFile();
};

#endif
