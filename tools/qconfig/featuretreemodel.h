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

#ifndef FEATURETREEMODEL_H
#define FEATURETREEMODEL_H

#include <QAbstractItemModel>
#include <QMap>
#include <QHash>
#include <QTextStream>

QT_BEGIN_NAMESPACE

class Feature;
class Node;

uint qHash(const QModelIndex&);

class FeatureTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FeatureTreeModel(QObject *parent = 0);
    ~FeatureTreeModel();

    void clear();
    
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(const Feature *feature) const;    
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void addFeature(Feature *feature);    
    Feature* getFeature(const QModelIndex &index) const;

    void readConfig(QTextStream &stream);
    void writeConfig(QTextStream &stream) const;
    
public slots:
    void featureChanged();
    
private:
    QModelIndex createIndex(int row, int column,
                            const QModelIndex &parent,
                            const Node *feature) const;    
    QModelIndex index(const QModelIndex &parent, const Feature *feature) const;
    bool contains(const QString &section, const Feature *f) const;
    Node* find(const QString &section, const Feature *f) const;
    QStringList findDisabled(const QModelIndex &parent) const;    
    
    QMap<QString, QList<Node*> > sections;
    mutable QHash<QModelIndex, QModelIndex> parentMap;
    mutable QHash<const Feature*, QModelIndex> featureIndexMap;    
};

QT_END_NAMESPACE

#endif // FEATURETREEMODEL_H
