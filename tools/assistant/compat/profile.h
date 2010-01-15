/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef PROFILE_H
#define PROFILE_H

#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QMap>

QT_BEGIN_NAMESPACE

class DocuParser;

class Profile
{
public:
    enum ProfileType { DefaultProfile, UserProfile };
    Profile();

    bool isValid() const;

    void addDCF( const QString &docfile );
    void addDCFIcon( const QString title, const QString &icon );
    void addDCFIndexPage( const QString title, const QString &indexPage );
    void addDCFImageDir( const QString title, const QString &imgDir );
    void addDCFTitle( const QString &dcf, const QString &title );
    void addProperty( const QString &name, const QString &value );
    bool hasDocFile( const QString &docFile );
    void removeDocFileEntry( const QString &title );

    ProfileType profileType() const { return type; }
    void setProfileType( ProfileType t ) { type = t; }

    DocuParser *docuParser() const { return dparser; }
    void setDocuParser( DocuParser *dp ) { dparser = dp; }

    static Profile* createDefaultProfile(const QString &docPath = QString());
    static QString makeRelativePath(const QString &base, const QString &path);
    static QString storableFilePath(const QString &fileName);
    static QString loadableFilePath(const QString &fileName);

    uint valid:1;
    ProfileType type;
    DocuParser *dparser;
    QMap<QString,QString> props;
    QMap<QString,QString> icons;
    QMap<QString,QString> indexPages;
    QMap<QString,QString> imageDirs;
    QMap<QString,QString> dcfTitles;
    QStringList docs;
};

QT_END_NAMESPACE

#endif // PROFILE_H
