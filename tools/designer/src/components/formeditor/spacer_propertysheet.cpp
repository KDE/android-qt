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

#include "spacer_propertysheet.h"
#include "qdesigner_widget_p.h"
#include "formwindow.h"
#include "spacer_widget_p.h"

#include <QtDesigner/QExtensionManager>

#include <QtGui/QLayout>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal
{
SpacerPropertySheet::SpacerPropertySheet(Spacer *object, QObject *parent)
    : QDesignerPropertySheet(object, parent)
{
    clearFakeProperties();
}

SpacerPropertySheet::~SpacerPropertySheet()
{
}

bool SpacerPropertySheet::isVisible(int index) const
{
    static const QString spacerGroup = QLatin1String("Spacer");
    return propertyGroup(index) == spacerGroup;
}

void SpacerPropertySheet::setProperty(int index, const QVariant &value)
{
    QDesignerPropertySheet::setProperty(index, value);
}

bool SpacerPropertySheet::dynamicPropertiesAllowed() const
{
    return false;
}
}

QT_END_NAMESPACE
