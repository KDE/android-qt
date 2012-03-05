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
#include "Test.h"

#include <QCoreApplication>
#include <QStringList>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QString arg;
    if (app.arguments().size() > 1)
        arg = app.arguments().at(1).toLower().trimmed();

    Test::Type type;
    if (arg == QLatin1String("qt4client"))
        type = Test::Qt4Client;
    else if (arg == QLatin1String("qt4server"))
        type = Test::Qt4Server;
#ifdef QT3_SUPPORT
    else if (arg == QLatin1String("qt3client"))
        type = Test::Qt3Client;
    else if (arg == QLatin1String("qt3server"))
        type = Test::Qt3Server;
#endif
    else {
        qDebug("usage: ./stressTest <qt3client|qt3server|qt4client|qt4server>");
        return 0;
    }

    Test test(type);
    
    return app.exec();
}
