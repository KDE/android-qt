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
#include <qtest.h>
#include <qdir.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QDebug>

class tst_qdeclarativemoduleplugin : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativemoduleplugin()
    {
    }

private slots:
    void importsPlugin();
    void incorrectPluginCase();
};

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

#define VERIFY_ERRORS(errorfile) \
    if (!errorfile) { \
        if (qgetenv("DEBUG") != "" && !component.errors().isEmpty()) \
            qWarning() << "Unexpected Errors:" << component.errors(); \
        QVERIFY(!component.isError()); \
        QVERIFY(component.errors().isEmpty()); \
    } else { \
        QFile file(QLatin1String("data/") + QLatin1String(errorfile)); \
        QVERIFY(file.open(QIODevice::ReadOnly)); \
        QByteArray data = file.readAll(); \
        file.close(); \
        QList<QByteArray> expected = data.split('\n'); \
        expected.removeAll(QByteArray("")); \
        QList<QDeclarativeError> errors = component.errors(); \
        QList<QByteArray> actual; \
        for (int ii = 0; ii < errors.count(); ++ii) { \
            const QDeclarativeError &error = errors.at(ii); \
            QByteArray errorStr = QByteArray::number(error.line()) + ":" +  \
                                  QByteArray::number(error.column()) + ":" + \
                                  error.description().toUtf8(); \
            actual << errorStr; \
        } \
        if (qgetenv("DEBUG") != "" && expected != actual) \
            qWarning() << "Expected:" << expected << "Actual:" << actual;  \
        if (qgetenv("QDECLARATIVELANGUAGE_UPDATEERRORS") != "" && expected != actual) {\
            QFile file(QLatin1String("data/") + QLatin1String(errorfile)); \
            QVERIFY(file.open(QIODevice::WriteOnly)); \
            for (int ii = 0; ii < actual.count(); ++ii) { \
                file.write(actual.at(ii)); file.write("\n"); \
            } \
            file.close(); \
        } else { \
            QCOMPARE(expected, actual); \
        } \
    }

inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath(filename));
}


void tst_qdeclarativemoduleplugin::importsPlugin()
{
    QDeclarativeEngine engine;
    engine.addImportPath(QLatin1String(SRCDIR) + QDir::separator() + QLatin1String("imports"));
    QTest::ignoreMessage(QtWarningMsg, "plugin created");
    QTest::ignoreMessage(QtWarningMsg, "import worked");
    QDeclarativeComponent component(&engine, TEST_FILE("data/works.qml"));
    foreach (QDeclarativeError err, component.errors())
    	qWarning() << err;
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("value").toInt(),123);
    delete object;
}

void tst_qdeclarativemoduleplugin::incorrectPluginCase()
{
    QDeclarativeEngine engine;
    engine.addImportPath(QLatin1String(SRCDIR) + QDir::separator() + QLatin1String("imports"));

    QDeclarativeComponent component(&engine, TEST_FILE("data/incorrectCase.qml"));

    QList<QDeclarativeError> errors = component.errors();
    QCOMPARE(errors.count(), 1);

#if defined(Q_OS_MAC) || defined(Q_OS_WIN32)
#if defined(Q_OS_MAC)
    QString libname = "libPluGin.dylib";
#elif defined(Q_OS_WIN32)
    QString libname = "PluGin.dll";
#endif
    QString expectedError = QLatin1String("plugin cannot be loaded for module \"com.nokia.WrongCase\": File name case mismatch for \"") + QFileInfo(__FILE__).absoluteDir().filePath("imports/com/nokia/WrongCase/" + libname) + QLatin1String("\"");
#else
    QString expectedError = QLatin1String("module \"com.nokia.WrongCase\" plugin \"PluGin\" not found");
#endif

    QCOMPARE(errors.at(0).description(), expectedError);
}

QTEST_MAIN(tst_qdeclarativemoduleplugin)

#include "tst_qdeclarativemoduleplugin.moc"
