/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qevent.h>
#ifdef QT_MAC_USE_COCOA
#import <Cocoa/Cocoa.h>

@class QT_MANGLE_NAMESPACE(QCocoaView);
QT_FORWARD_DECLARE_CLASS(QWidgetPrivate);
QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QEvent);
QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QStringList);

Q_GUI_EXPORT
@interface QT_MANGLE_NAMESPACE(QCocoaView) : NSControl <NSTextInput> {
    QWidget *qwidget;
    QWidgetPrivate *qwidgetprivate;
    NSDragOperation supportedActions;
    bool composing;
    int composingLength;
    bool sendKeyEvents;
    bool fromKeyDownEvent;
    QString *composingText;
    @public int alienTouchCount;
}
- (id)initWithQWidget:(QWidget *)widget widgetPrivate:(QWidgetPrivate *)widgetprivate;
- (void) finishInitWithQWidget:(QWidget *)widget widgetPrivate:(QWidgetPrivate *)widgetprivate;
- (void)frameDidChange:(NSNotification *)note;
- (void)setSupportedActions:(NSDragOperation)actions;
- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)isLocal;
- (void)draggedImage:(NSImage *)anImage endedAt:(NSPoint)aPoint operation:(NSDragOperation)operation;
- (BOOL)isComposing;
- (QWidget *)qt_qwidget;
- (void) qt_clearQWidget;

@end
#endif
