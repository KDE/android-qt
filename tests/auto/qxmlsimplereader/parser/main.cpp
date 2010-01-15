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


#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>

#include "parser.h"

static QTextStream qout(stdout, QIODevice::WriteOnly);
static QTextStream qerr(stderr, QIODevice::WriteOnly);

static void usage()
{
    qerr << "Usage: parse [-report-whitespace-only-chardata] [-report-start-end-entity] <in-file> [<out-file>]\n";
    exit(1);
}

int main(int argc, const char *argv[])
{
    QString file_name;
    QString out_file_name;
    bool report_start_end_entity = false;
    bool report_whitespace_only_chardata = false;

    for (int i = 1 ; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg == QLatin1String("-report-whitespace-only-chardata"))
            report_whitespace_only_chardata = true;
        else if (arg == QLatin1String("-report-start-end-entity"))
            report_start_end_entity = true;
        else if (file_name.isEmpty())
            file_name = arg;
        else if (out_file_name.isEmpty())
            out_file_name = arg;
        else
            usage();
    }

    if (file_name.isEmpty())
        usage();

    QFile in_file(file_name);
    if (!in_file.open(QIODevice::ReadOnly)) {
        qerr << "Could not open " << file_name << ": " << strerror(errno) << endl;
        return 1;
    }

    if (out_file_name.isEmpty())
        out_file_name = file_name + ".ref";

    QFile _out_file;
    QTextStream _out_stream;
    QTextStream *out_stream;
    if (out_file_name == "-") {
        out_stream = &qout;
    } else {
        _out_file.setFileName(out_file_name);
        if (!_out_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qerr << "Could not open " << out_file_name << ": " << strerror(errno) << endl;
            return 1;
        }
        _out_stream.setDevice(&_out_file);
        out_stream = &_out_stream;
    }

    Parser parser;
    if (report_start_end_entity)
        parser.setFeature("http://trolltech.com/xml/features/report-start-end-entity", true);
    if (report_whitespace_only_chardata)
        parser.setFeature("http://trolltech.com/xml/features/report-whitespace-only-CharData", true);

    parser.parseFile(&in_file);

    out_stream->setCodec("utf8");

    *out_stream << parser.result();

    return 0;
}
