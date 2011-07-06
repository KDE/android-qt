/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include "../shared/shared.h"

int main(int argc, char **argv)
{
    // useDebugLibs should always be false because even if set all Qt
    // libraries inside a binary to point to debug versions, as soon as
    // one of them loads a Qt plugin, the plugin itself will load the
    // release version of Qt, and as such, the app will crash.
    bool useDebugLibs = false;

    int optionsSpecified = 0;
    for (int i = 2; i < argc; ++i) {
        QByteArray argument = QByteArray(argv[i]);
        if (argument.startsWith(QByteArray("-verbose="))) {
            LogDebug() << "Argument found:" << argument;
            optionsSpecified++;
            int index = argument.indexOf("=");
            bool ok = false;
            int number = argument.mid(index+1).toInt(&ok);
            if (!ok)
                LogError() << "Could not parse verbose level";
            else
                logLevel = number;
        }
    }

    if (argc != (3 + optionsSpecified)) {
        qDebug() << "Changeqt: changes witch Qt frameworks an application links against.";
        qDebug() << "Usage: changeqt app-bundle qt-dir <-verbose=[0-3]>";
        return 0;
    }

    const QString appPath = QString::fromLocal8Bit(argv[1]);
    const QString qtPath = QString::fromLocal8Bit(argv[2]);
    changeQtFrameworks(appPath, qtPath, useDebugLibs);
}
