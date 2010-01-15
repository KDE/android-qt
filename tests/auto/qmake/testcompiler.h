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
#ifndef TESTCOMPILER_H
#define TESTCOMPILER_H

#include <QObject>
#include <QStringList>

enum BuildType { Exe, Dll, Lib, Plain };

class TestCompiler : public QObject
{
    Q_OBJECT

public:
    TestCompiler();
    virtual ~TestCompiler();

    void setBaseCommands( QString makeCmd, QString qmakeCmd );
    void resetEnvironment();
    void addToEnvironment( QString varAssignment );

    // executes a make clean in the specified workPath
    bool makeClean( const QString &workPath );
    // executes a make dist clean in the specified workPath
    bool makeDistClean( const QString &workPath );
    // executes a qmake on proName in the specified workDir, output goes to buildDir or workDir if it's null
    bool qmake( const QString &workDir, const QString &proName, const QString &buildDir = QString() );
    // executes a make in the specified workPath, with an optional target (eg. install)
    bool make( const QString &workPath, const QString &target = QString() );
    // checks if the executable exists in destDir
    bool exists( const QString &destDir, const QString &exeName, BuildType buildType, const QString &version );
    // removes the makefile
    bool removeMakefile( const QString &workPath );
    // returns each line of stdout of the last command append with a "new line" character(s) to suit the platform
    QString commandOutput() const;
    // clear the results of storage of stdout for running previous commands
    void clearCommandOutput();

private:
    bool runCommand( QString cmdLine );

    QString makeCmd_;
    QString qmakeCmd_;
    QStringList environment_;

    // need to make this available somewhere
    QStringList testOutput_;
};

#endif // TESTCOMPILER_H
