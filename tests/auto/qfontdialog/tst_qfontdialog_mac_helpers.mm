/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <private/qt_mac_p.h>
#include <AppKit/AppKit.h>

void click_cocoa_button()
{
    QMacCocoaAutoReleasePool pool;
    NSArray *windows = [NSApp windows];
    for (NSWindow *window in windows) {
        // This is NOT how one should do RTTI, but since I don't want to leak the class too much...
        if ([[window delegate] respondsToSelector:@selector(qtFont)]) {
            NSArray *subviews = [[window contentView] subviews];
            for (NSView *view in subviews) {
                if ([view isKindOfClass:[NSButton class]]
                        && [[static_cast<NSButton *>(view) title] isEqualTo:@"OK"]) {
                    [static_cast<NSButton *>(view) performClick:view];
                    [NSApp postEvent:[NSEvent otherEventWithType:NSApplicationDefined location:NSZeroPoint
                        modifierFlags:0 timestamp:0. windowNumber:0 context:0
                        subtype:SHRT_MAX data1:0 data2:0] atStart:NO];

                    break;
                }
            }
            break;
        }
    }
}
