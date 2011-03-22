/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FEATURE_H
#define FEATURE_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QList>

QT_BEGIN_NAMESPACE

class Feature;

class FeaturePrivate
{
public:
    FeaturePrivate(const QString &k)
	: key(k), enabled(true), selectable(true) {};
    
    const QString key;
    QString section;
    QString title;
    QString description;
    QSet<Feature*> dependencies;
    QSet<Feature*> supports; // features who depends on this one
    QSet<Feature*> relations;
    bool enabled;
    bool selectable;
};

class Feature : public QObject
{
    Q_OBJECT    

public:
    static Feature* getInstance(const QString &key);
    static void clear();

public:
    QString key() const { return d->key; }

    void setTitle(const QString &title);
    QString title() const { return d->title; }

    void setSection(const QString &section);
    QString section() const { return d->section; }

    void setDescription(const QString &description);    
    QString description() const { return d->description; };

    void addRelation(const QString &key);
    void setRelations(const QStringList &keys);
    QList<Feature*> relations() const;    

    void addDependency(const QString &dependency);
    void setDependencies(const QStringList &dependencies);
    QList<Feature*> dependencies() const;

    QList<Feature*> supports() const;    
    QString getDocumentation() const;

    void setEnabled(bool on);
    bool enabled() const { return d->enabled; };

    bool selectable() const { return d->selectable; }
    
    QString toHtml() const;    
    
    ~Feature();    

signals:
    void changed();
    
private:
    Feature(const QString &key);
    void updateSelectable();
    
    static QMap<QString, Feature*> instances;
    FeaturePrivate *d;
};

QT_END_NAMESPACE

#endif // FEATURE_H
