/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef BRUSHPROPERTYMANAGER_H
#define BRUSHPROPERTYMANAGER_H

#include <QtCore/QMap>
#include <QtGui/QBrush>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

class QtProperty;
class QtVariantPropertyManager;

class QString;
class QVariant;

namespace qdesigner_internal {

// BrushPropertyManager: A mixin for DesignerPropertyManager that manages brush properties.

class BrushPropertyManager {
    BrushPropertyManager(const BrushPropertyManager&);
    BrushPropertyManager &operator=(const BrushPropertyManager&);

public:
    BrushPropertyManager();

    void initializeProperty(QtVariantPropertyManager *vm, QtProperty *property, int enumTypeId);
    bool uninitializeProperty(QtProperty *property);

    // Call from slotValueChanged().
    enum ValueChangedResult { NoMatch, Unchanged, Changed };
    ValueChangedResult valueChanged(QtVariantPropertyManager *vm, QtProperty *property, const QVariant &value);
    ValueChangedResult setValue(QtVariantPropertyManager *vm, QtProperty *property, const QVariant &value);

    bool valueText(const QtProperty *property, QString *text) const;
    bool valueIcon(const QtProperty *property, QIcon *icon) const;
    bool value(const QtProperty *property, QVariant *v) const;

    // Call from  QtPropertyManager's propertyDestroyed signal
    void slotPropertyDestroyed(QtProperty *property);

private:
    static int brushStyleToIndex(Qt::BrushStyle st);
    static Qt::BrushStyle brushStyleIndexToStyle(int brushStyleIndex);
    static QString brushStyleIndexToString(int brushStyleIndex);

    typedef QMap<int, QIcon> EnumIndexIconMap;
    static const EnumIndexIconMap &brushStyleIcons();

    typedef QMap<QtProperty *, QtProperty *> PropertyToPropertyMap;
    PropertyToPropertyMap m_brushPropertyToStyleSubProperty;
    PropertyToPropertyMap m_brushPropertyToColorSubProperty;
    PropertyToPropertyMap m_brushStyleSubPropertyToProperty;
    PropertyToPropertyMap m_brushColorSubPropertyToProperty;

    typedef QMap<QtProperty *, QBrush> PropertyBrushMap;
    PropertyBrushMap m_brushValues;
};

}

QT_END_NAMESPACE

#endif // BRUSHPROPERTYMANAGER_H
