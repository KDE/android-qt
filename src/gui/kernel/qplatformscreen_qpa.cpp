/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qplatformscreen_qpa.h"
#include <QtGui/qapplication.h>
#include <QtGui/qdesktopwidget.h>

QWidget *QPlatformScreen::topLevelAt(const QPoint & pos) const
{
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i = list.size()-1; i >= 0; --i) {
        QWidget *w = list[i];
        //### mask is ignored
        if (w != QApplication::desktop() && w->isVisible() && w->geometry().contains(pos))
            return w;
    }

    return 0;
}

QSize QPlatformScreen::physicalSize() const
{
    static const int dpi = 100;
    int width = geometry().width() / dpi * qreal(25.4) ;
    int height = geometry().height() / dpi * qreal(25.4) ;
    return QSize(width,height);
}

