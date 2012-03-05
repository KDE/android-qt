/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "framework.h"

#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStringList>
#include <QtDebug>

Framework::Framework()
    : qsettings(0)
{
}

Framework::Framework(const QString &file)
    : qsettings(0)
{
    load(file);
}

Framework::~Framework()
{
    delete qsettings;
    qsettings = 0;
}

QString Framework::basePath() const
{
    if (!qsettings)
        return QString();

    QFileInfo fi(qsettings->fileName());
    return fi.absolutePath();
}


QStringList Framework::suites() const
{
    if (!qsettings)
        return QStringList();

    QStringList tests = qsettings->childGroups();
    qDebug()<<"here suites "<<tests;
    tests.removeAll("General");
    tests.removeAll("Blacklist");
    return tests;
}


bool Framework::isTestBlacklisted(const QString &engineName,
                                  const QString &testcase) const
{
    return m_blacklist[engineName].contains(testcase);
}

bool Framework::isValid() const
{
    return qsettings;
}

void Framework::load(const QString &file)
{
    if (qsettings) {
        delete qsettings;
        qsettings = 0;
    }
    if (QFile::exists(file)) {
        qsettings = new QSettings(file, QSettings::IniFormat);
        qsettings->beginGroup(QString("Blacklist"));
        QStringList engines = qsettings->childKeys();
        foreach(QString engineName, engines) {
            QStringList testcases = qsettings->value(engineName).toStringList();
            m_blacklist.insert(engineName, testcases);
            qDebug()<<"Blacklists for "<<testcases;
        }
        qsettings->endGroup();
    }
}

QString Framework::outputDir() const
{
    qsettings->beginGroup("General");
    QString outputDirName = qsettings->value("outputDir").toString();
    qsettings->endGroup();
    return outputDirName;
}

QSettings * Framework::settings() const
{
    return qsettings;
}
