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

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include "dcfsection.h"
#include "htmlgenerator.h"

QT_BEGIN_NAMESPACE

void appendDcfSubSection( DcfSection *dcfSect, const DcfSection& sub )
{
    dcfSect->subsections.append( sub );
}

void appendDcfSubSections( DcfSection *dcfSect, const QList<DcfSection>& subs )
{
    dcfSect->subsections += subs;
}

void generateDcfSubSections( QString indent, QTextStream& out, const DcfSection& sect )
{
    QList<DcfSection>::const_iterator ss = sect.subsections.constBegin();
    while ( ss != sect.subsections.constEnd() ) {
	out << indent << "<section ref=\"" << HtmlGenerator::cleanRef(HtmlGenerator::protect((*ss).ref))
            << "\" title=\"" << HtmlGenerator::protect((*ss).title) << "\"";
	if ((*ss).keywords.isEmpty() && (*ss).subsections.isEmpty()) {
	    out << "/>\n";
	} else {
	    out << ">\n";
	    QString indentIndent = indent + "    ";
	    QList<QPair<QString, QString> >::const_iterator k = (*ss).keywords.constBegin();
	    while ( k != (*ss).keywords.constEnd() ) {
		out << indentIndent << "<keyword ref=\"" << HtmlGenerator::cleanRef((*k).second) << "\">"
                    << HtmlGenerator::protect((*k).first) << "</keyword>\n";
		++k;
	    }

            generateDcfSubSections( indentIndent, out, *ss );
	    out << indent << "</section>\n";
	}
	++ss;
    }
    out.flush();
}

void generateDcfSections( const DcfSection& rootSect, const QString& fileName,
			  const QString& /* category */ )
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return ;

    QTextStream out(&file);

    QString icon = QFileInfo(fileName).baseName() + ".png";

    out << "<!DOCTYPE DCF>\n";
    out << "<DCF ref=\"" << HtmlGenerator::cleanRef(HtmlGenerator::protect(rootSect.ref));
    if (icon != "qmake.png")
	out << "\" icon=\"" << HtmlGenerator::protect(icon);
    out << "\" imagedir=\"../../gif\" title=\"" << HtmlGenerator::protect(rootSect.title) +
	      "\">\n";

    generateDcfSubSections( "", out, rootSect );

    out << "</DCF>\n";
    out.flush();
}

QT_END_NAMESPACE
