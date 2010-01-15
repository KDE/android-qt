/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "qtestxmlstreamer.h"
#include "qtestelement.h"
#include "qtestelementattribute.h"

#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qxmltestlogger_p.h"

#include <string.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

QTestXmlStreamer::QTestXmlStreamer()
    :QTestBasicStreamer()
{
}

QTestXmlStreamer::~QTestXmlStreamer()
{}

void QTestXmlStreamer::formatStart(const QTestElement *element, QTestCharBuffer *formatted) const
{
    if(!element || !formatted)
        return;

    switch(element->elementType()){
    case QTest::LET_TestCase: {
        QTestCharBuffer quotedTf;
        QXmlTestLogger::xmlQuote(&quotedTf, element->attributeValue(QTest::AI_Name));

        QTest::qt_asprintf(formatted, "<TestFunction name=\"%s\">\n", quotedTf.constData());
        break;
    }
    case QTest::LET_Failure: {
        QTestCharBuffer cdataDesc;
        QXmlTestLogger::xmlCdata(&cdataDesc, element->attributeValue(QTest::AI_Description));

        QTestCharBuffer location;
        QTestCharBuffer quotedFile;
        QXmlTestLogger::xmlQuote(&quotedFile, element->attributeValue(QTest::AI_File));

        QTest::qt_asprintf(&location, "%s=\"%s\" %s=\"%s\"",
                           element->attributeName(QTest::AI_File),
                           quotedFile.constData(),
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line));

        if (element->attribute(QTest::AI_Tag)) {
            QTestCharBuffer cdataTag;
            QXmlTestLogger::xmlCdata(&cdataTag, element->attributeValue(QTest::AI_Tag));
            QTest::qt_asprintf(formatted, "<Incident type=\"%s\" %s>\n"
                "    <DataTag><![CDATA[%s]]></DataTag>\n"
                "    <Description><![CDATA[%s]]></Description>\n"
                "</Incident>\n", element->attributeValue(QTest::AI_Result),
                location.constData(), cdataTag.constData(), cdataDesc.constData());
        }
        else {
            QTest::qt_asprintf(formatted, "<Incident type=\"%s\" %s>\n"
                "    <Description><![CDATA[%s]]></Description>\n"
                "</Incident>\n", element->attributeValue(QTest::AI_Result),
                location.constData(), cdataDesc.constData());
        }
        break;
    }
    case QTest::LET_Error: {
        // assuming type and attribute names don't need quoting
        QTestCharBuffer quotedFile;
        QTestCharBuffer cdataDesc;
        QXmlTestLogger::xmlQuote(&quotedFile, element->attributeValue(QTest::AI_File));
        QXmlTestLogger::xmlCdata(&cdataDesc, element->attributeValue(QTest::AI_Description));

        QTest::qt_asprintf(formatted, "<Message type=\"%s\" %s=\"%s\" %s=\"%s\">\n    <Description><![CDATA[%s]]></Description>\n</Message>\n",
                           element->attributeValue(QTest::AI_Type),
                           element->attributeName(QTest::AI_File),
                           quotedFile.constData(),
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line),
                           cdataDesc.constData());
        break;
    }
    case QTest::LET_Benchmark: {
        // assuming value and iterations don't need quoting
        QTestCharBuffer quotedMetric;
        QTestCharBuffer quotedTag;
        QXmlTestLogger::xmlQuote(&quotedMetric, element->attributeValue(QTest::AI_Metric));
        QXmlTestLogger::xmlQuote(&quotedTag, element->attributeValue(QTest::AI_Tag));

        QTest::qt_asprintf(formatted, "<BenchmarkResult %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\n",
                           element->attributeName(QTest::AI_Metric),
                           quotedMetric.constData(),
                           element->attributeName(QTest::AI_Tag),
                           quotedTag.constData(),
                           element->attributeName(QTest::AI_Value),
                           element->attributeValue(QTest::AI_Value),
                           element->attributeName(QTest::AI_Iterations),
                           element->attributeValue(QTest::AI_Iterations) );
        break;
    }
    default:
        formatted->data()[0] = '\0';
    }
}

void QTestXmlStreamer::formatEnd(const QTestElement *element, QTestCharBuffer *formatted) const
{
    if(!element || !formatted)
        return;

    if (element->elementType() == QTest::LET_TestCase) {
        QTest::qt_asprintf(formatted, "</TestFunction>\n");
    } else {
        formatted->data()[0] = '\0';
    }
}

void QTestXmlStreamer::formatBeforeAttributes(const QTestElement *element, QTestCharBuffer *formatted) const
{
    if(!element || !formatted)
        return;

    if (element->elementType() == QTest::LET_TestCase && element->attribute(QTest::AI_Result)){
        QTestCharBuffer buf;
        QTestCharBuffer quotedFile;
        QXmlTestLogger::xmlQuote(&quotedFile, element->attributeValue(QTest::AI_File));

        QTest::qt_asprintf(&buf, "%s=\"%s\" %s=\"%s\"",
                           element->attributeName(QTest::AI_File),
                           quotedFile.constData(),
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line));

        if( !element->childElements() ) {
            QTest::qt_asprintf(formatted, "<Incident type=\"%s\" %s/>\n",
                               element->attributeValue(QTest::AI_Result), buf.constData());
        } else {
            formatted->data()[0] = '\0';
        }
    } else {
        formatted->data()[0] = '\0';
    }
}

void QTestXmlStreamer::output(QTestElement *element) const
{
    QTestCharBuffer buf;
    QTestCharBuffer quotedTc;
    QXmlTestLogger::xmlQuote(&quotedTc, QTestResult::currentTestObjectName());

    QTest::qt_asprintf(&buf, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<TestCase name=\"%s\">\n",
                       quotedTc.constData());
    outputString(buf.constData());

    QTest::qt_asprintf(&buf, "<Environment>\n    <QtVersion>%s</QtVersion>\n    <QTestVersion>%s</QTestVersion>\n",
                       qVersion(), QTEST_VERSION_STR );
    outputString(buf.constData());

    QTest::qt_asprintf(&buf, "</Environment>\n");
    outputString(buf.constData());

    QTestBasicStreamer::output(element);

    QTest::qt_asprintf(&buf, "</TestCase>\n");
    outputString(buf.constData());
}

QT_END_NAMESPACE

