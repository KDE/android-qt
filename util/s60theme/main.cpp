/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtGui>
#include "s60themeconvert.h"

int help()
{
    qDebug() << "Usage: s60theme [modeldir|theme.tdf] output.blob";
    qDebug() << "";
    qDebug() << "Options:";
    qDebug() << "   modeldir:    Theme 'model' directory in Carbide.ui tree";
    qDebug() << "   theme.tdf:   Theme .tdf file";
    qDebug() << "   output.blob: Theme blob output filename";
    qDebug() << "";
    qDebug() << "s60theme takes an S60 theme from Carbide.ui and converts";
    qDebug() << "it into a compact, binary format, that can be directly loaded by";
    qDebug() << "the QtS60Style.";
    qDebug() << "";
    qDebug() << "Visit http://www.forum.nokia.com for details about Carbide.ui";
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        return help();

    QApplication app(argc, argv);

    const QString input = QString::fromLatin1(argv[1]);
    const QFileInfo inputInfo(input);
    const QString output = QString::fromLatin1(argv[2]);
    if (inputInfo.isDir())
        return S60ThemeConvert::convertDefaultThemeToBlob(input, output);
    else if (inputInfo.suffix().compare(QString::fromLatin1("tdf"), Qt::CaseInsensitive) == 0)
        return S60ThemeConvert::convertTdfToBlob(input, output);

    return help();
}
