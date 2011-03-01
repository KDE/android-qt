/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qxcbconnection.h"
#include "qxcbkeyboard.h"
#include "qxcbscreen.h"
#include "qxcbwindow.h"

#include <QtAlgorithms>
#include <QSocketNotifier>

#include <stdio.h>

#ifdef XCB_USE_XLIB_FOR_GLX
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#endif

QXcbConnection::QXcbConnection(const char *displayName)
    : m_displayName(displayName ? QByteArray(displayName) : qgetenv("DISPLAY"))
{
    int primaryScreen = 0;

#ifdef XCB_USE_XLIB_FOR_GLX
    Display *dpy = XOpenDisplay(m_displayName.constData());
    primaryScreen = DefaultScreen(dpy);
    m_connection = XGetXCBConnection(dpy);
    XSetEventQueueOwner(dpy, XCBOwnsEventQueue);
    m_xlib_display = dpy;
#else
    m_connection = xcb_connect(m_displayName.constData(), &primaryScreen);
#endif

    m_setup = xcb_get_setup(xcb_connection());

    xcb_screen_iterator_t it = xcb_setup_roots_iterator(m_setup);

    int screenNumber = 0;
    while (it.rem) {
        m_screens << new QXcbScreen(this, it.data, screenNumber++);
        xcb_screen_next(&it);
    }

    QSocketNotifier *socket = new QSocketNotifier(xcb_get_file_descriptor(xcb_connection()), QSocketNotifier::Read, this);
    connect(socket, SIGNAL(activated(int)), this, SLOT(eventDispatcher()));

    m_keyboard = new QXcbKeyboard(this);

    initializeAllAtoms();
}

QXcbConnection::~QXcbConnection()
{
    qDeleteAll(m_screens);

#ifdef XCB_USE_XLIB_FOR_GLX
    XCloseDisplay((Display *)m_xlib_display);
#else
    xcb_disconnect(xcb_connection());
#endif

    delete m_keyboard;
}

QXcbWindow *platformWindowFromId(xcb_window_t id)
{
    QWidget *widget = QWidget::find(id);
    if (widget)
        return static_cast<QXcbWindow *>(widget->platformWindow());
    return 0;
}

#define HANDLE_PLATFORM_WINDOW_EVENT(event_t, window, handler) \
{ \
    event_t *e = (event_t *)event; \
    if (QXcbWindow *platformWindow = platformWindowFromId(e->window)) \
        platformWindow->handler(e); \
} \
break;

#define HANDLE_KEYBOARD_EVENT(event_t, handler) \
{ \
    event_t *e = (event_t *)event; \
    if (QXcbWindow *platformWindow = platformWindowFromId(e->event)) \
        m_keyboard->handler(platformWindow->widget(), e); \
} \
break;

#define XCB_EVENT_DEBUG

void printXcbEvent(const char *message, xcb_generic_event_t *event)
{
#ifdef XCB_EVENT_DEBUG
#define PRINT_XCB_EVENT(event) \
    case event: \
        printf("%s: %d - %s\n", message, event, #event); \
        break;

    switch (event->response_type & ~0x80) {
    PRINT_XCB_EVENT(XCB_KEY_PRESS);
    PRINT_XCB_EVENT(XCB_KEY_RELEASE);
    PRINT_XCB_EVENT(XCB_BUTTON_PRESS);
    PRINT_XCB_EVENT(XCB_BUTTON_RELEASE);
    PRINT_XCB_EVENT(XCB_MOTION_NOTIFY);
    PRINT_XCB_EVENT(XCB_ENTER_NOTIFY);
    PRINT_XCB_EVENT(XCB_LEAVE_NOTIFY);
    PRINT_XCB_EVENT(XCB_FOCUS_IN);
    PRINT_XCB_EVENT(XCB_FOCUS_OUT);
    PRINT_XCB_EVENT(XCB_KEYMAP_NOTIFY);
    PRINT_XCB_EVENT(XCB_EXPOSE);
    PRINT_XCB_EVENT(XCB_GRAPHICS_EXPOSURE);
    PRINT_XCB_EVENT(XCB_VISIBILITY_NOTIFY);
    PRINT_XCB_EVENT(XCB_CREATE_NOTIFY);
    PRINT_XCB_EVENT(XCB_DESTROY_NOTIFY);
    PRINT_XCB_EVENT(XCB_UNMAP_NOTIFY);
    PRINT_XCB_EVENT(XCB_MAP_NOTIFY);
    PRINT_XCB_EVENT(XCB_MAP_REQUEST);
    PRINT_XCB_EVENT(XCB_REPARENT_NOTIFY);
    PRINT_XCB_EVENT(XCB_CONFIGURE_NOTIFY);
    PRINT_XCB_EVENT(XCB_CONFIGURE_REQUEST);
    PRINT_XCB_EVENT(XCB_GRAVITY_NOTIFY);
    PRINT_XCB_EVENT(XCB_RESIZE_REQUEST);
    PRINT_XCB_EVENT(XCB_CIRCULATE_NOTIFY);
    PRINT_XCB_EVENT(XCB_CIRCULATE_REQUEST);
    PRINT_XCB_EVENT(XCB_PROPERTY_NOTIFY);
    PRINT_XCB_EVENT(XCB_SELECTION_CLEAR);
    PRINT_XCB_EVENT(XCB_SELECTION_REQUEST);
    PRINT_XCB_EVENT(XCB_SELECTION_NOTIFY);
    PRINT_XCB_EVENT(XCB_COLORMAP_NOTIFY);
    PRINT_XCB_EVENT(XCB_CLIENT_MESSAGE);
    PRINT_XCB_EVENT(XCB_MAPPING_NOTIFY);
    default:
        printf("%s: %d - %s\n", message, event->response_type, "unknown");
    }
#else
    Q_UNUSED(message);
    Q_UNUSED(event);
#endif
}

void QXcbConnection::eventDispatcher()
{
    while (xcb_generic_event_t *event = xcb_poll_for_event(xcb_connection())) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_expose_event_t, window, handleExposeEvent);
        case XCB_BUTTON_PRESS:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_button_press_event_t, event, handleButtonPressEvent);
        case XCB_BUTTON_RELEASE:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_button_release_event_t, event, handleButtonReleaseEvent);
        case XCB_MOTION_NOTIFY:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_motion_notify_event_t, event, handleMotionNotifyEvent);
        case XCB_CONFIGURE_NOTIFY:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_configure_notify_event_t, event, handleConfigureNotifyEvent);
        case XCB_CLIENT_MESSAGE:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_client_message_event_t, window, handleClientMessageEvent);
        case XCB_ENTER_NOTIFY:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_enter_notify_event_t, event, handleEnterNotifyEvent);
        case XCB_LEAVE_NOTIFY:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_leave_notify_event_t, event, handleLeaveNotifyEvent);
        case XCB_FOCUS_IN:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_focus_in_event_t, event, handleFocusInEvent);
        case XCB_FOCUS_OUT:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_focus_out_event_t, event, handleFocusOutEvent);
        case XCB_KEY_PRESS:
            HANDLE_KEYBOARD_EVENT(xcb_key_press_event_t, handleKeyPressEvent);
        case XCB_KEY_RELEASE:
            HANDLE_KEYBOARD_EVENT(xcb_key_release_event_t, handleKeyReleaseEvent);
        case XCB_MAPPING_NOTIFY:
            m_keyboard->handleMappingNotifyEvent((xcb_mapping_notify_event_t *)event);
            break;
        default:
            printXcbEvent("Unhandled XCB event", event);
            return;
        }
        printXcbEvent("Handled XCB event", event);
    }
}

static const char * xcb_atomnames = {
    // window-manager <-> client protocols
    "WM_PROTOCOLS\0"
    "WM_DELETE_WINDOW\0"
    "WM_TAKE_FOCUS\0"
    "_NET_WM_PING\0"
    "_NET_WM_CONTEXT_HELP\0"
    "_NET_WM_SYNC_REQUEST\0"
    "_NET_WM_SYNC_REQUEST_COUNTER\0"

    // ICCCM window state
    "WM_STATE\0"
    "WM_CHANGE_STATE\0"

    // Session management
    "WM_CLIENT_LEADER\0"
    "WM_WINDOW_ROLE\0"
    "SM_CLIENT_ID\0"

    // Clipboard
    "CLIPBOARD\0"
    "INCR\0"
    "TARGETS\0"
    "MULTIPLE\0"
    "TIMESTAMP\0"
    "SAVE_TARGETS\0"
    "CLIP_TEMPORARY\0"
    "_QT_SELECTION\0"
    "_QT_CLIPBOARD_SENTINEL\0"
    "_QT_SELECTION_SENTINEL\0"
    "CLIPBOARD_MANAGER\0"

    "RESOURCE_MANAGER\0"

    "_XSETROOT_ID\0"

    "_QT_SCROLL_DONE\0"
    "_QT_INPUT_ENCODING\0"

    "_MOTIF_WM_HINTS\0"

    "DTWM_IS_RUNNING\0"
    "ENLIGHTENMENT_DESKTOP\0"
    "_DT_SAVE_MODE\0"
    "_SGI_DESKS_MANAGER\0"

    // EWMH (aka NETWM)
    "_NET_SUPPORTED\0"
    "_NET_VIRTUAL_ROOTS\0"
    "_NET_WORKAREA\0"

    "_NET_MOVERESIZE_WINDOW\0"
    "_NET_WM_MOVERESIZE\0"

    "_NET_WM_NAME\0"
    "_NET_WM_ICON_NAME\0"
    "_NET_WM_ICON\0"

    "_NET_WM_PID\0"

    "_NET_WM_WINDOW_OPACITY\0"

    "_NET_WM_STATE\0"
    "_NET_WM_STATE_ABOVE\0"
    "_NET_WM_STATE_BELOW\0"
    "_NET_WM_STATE_FULLSCREEN\0"
    "_NET_WM_STATE_MAXIMIZED_HORZ\0"
    "_NET_WM_STATE_MAXIMIZED_VERT\0"
    "_NET_WM_STATE_MODAL\0"
    "_NET_WM_STATE_STAYS_ON_TOP\0"
    "_NET_WM_STATE_DEMANDS_ATTENTION\0"

    "_NET_WM_USER_TIME\0"
    "_NET_WM_USER_TIME_WINDOW\0"
    "_NET_WM_FULL_PLACEMENT\0"

    "_NET_WM_WINDOW_TYPE\0"
    "_NET_WM_WINDOW_TYPE_DESKTOP\0"
    "_NET_WM_WINDOW_TYPE_DOCK\0"
    "_NET_WM_WINDOW_TYPE_TOOLBAR\0"
    "_NET_WM_WINDOW_TYPE_MENU\0"
    "_NET_WM_WINDOW_TYPE_UTILITY\0"
    "_NET_WM_WINDOW_TYPE_SPLASH\0"
    "_NET_WM_WINDOW_TYPE_DIALOG\0"
    "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU\0"
    "_NET_WM_WINDOW_TYPE_POPUP_MENU\0"
    "_NET_WM_WINDOW_TYPE_TOOLTIP\0"
    "_NET_WM_WINDOW_TYPE_NOTIFICATION\0"
    "_NET_WM_WINDOW_TYPE_COMBO\0"
    "_NET_WM_WINDOW_TYPE_DND\0"
    "_NET_WM_WINDOW_TYPE_NORMAL\0"
    "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE\0"

    "_KDE_NET_WM_FRAME_STRUT\0"

    "_NET_STARTUP_INFO\0"
    "_NET_STARTUP_INFO_BEGIN\0"

    "_NET_SUPPORTING_WM_CHECK\0"

    "_NET_WM_CM_S0\0"

    "_NET_SYSTEM_TRAY_VISUAL\0"

    "_NET_ACTIVE_WINDOW\0"

    // Property formats
    "COMPOUND_TEXT\0"
    "TEXT\0"
    "UTF8_STRING\0"

    // xdnd
    "XdndEnter\0"
    "XdndPosition\0"
    "XdndStatus\0"
    "XdndLeave\0"
    "XdndDrop\0"
    "XdndFinished\0"
    "XdndTypeList\0"
    "XdndActionList\0"

    "XdndSelection\0"

    "XdndAware\0"
    "XdndProxy\0"

    "XdndActionCopy\0"
    "XdndActionLink\0"
    "XdndActionMove\0"
    "XdndActionPrivate\0"

    // Motif DND
    "_MOTIF_DRAG_AND_DROP_MESSAGE\0"
    "_MOTIF_DRAG_INITIATOR_INFO\0"
    "_MOTIF_DRAG_RECEIVER_INFO\0"
    "_MOTIF_DRAG_WINDOW\0"
    "_MOTIF_DRAG_TARGETS\0"

    "XmTRANSFER_SUCCESS\0"
    "XmTRANSFER_FAILURE\0"

    // Xkb
    "_XKB_RULES_NAMES\0"

    // XEMBED
    "_XEMBED\0"
    "_XEMBED_INFO\0"

    // Wacom old. (before version 0.10)
    "Wacom Stylus\0"
    "Wacom Cursor\0"
    "Wacom Eraser\0"

    // Tablet
    "STYLUS\0"
    "ERASER\0"
};

xcb_atom_t QXcbConnection::atom(QXcbAtom::Atom atom)
{
    return m_allAtoms[atom];
}

void QXcbConnection::initializeAllAtoms() {
    const char *names[QXcbAtom::NAtoms];
    const char *ptr = xcb_atomnames;

    int i = 0;
    while (*ptr) {
        names[i++] = ptr;
        while (*ptr)
            ++ptr;
        ++ptr;
    }

    Q_ASSERT(i == QXcbAtom::NPredefinedAtoms);

    QByteArray settings_atom_name("_QT_SETTINGS_TIMESTAMP_");
    settings_atom_name += m_displayName;
    names[i++] = settings_atom_name;

    xcb_intern_atom_cookie_t cookies[QXcbAtom::NAtoms];

    Q_ASSERT(i == QXcbAtom::NAtoms);
    for (i = 0; i < QXcbAtom::NAtoms; ++i)
        cookies[i] = xcb_intern_atom(xcb_connection(), false, strlen(names[i]), names[i]);

    for (i = 0; i < QXcbAtom::NAtoms; ++i)
        m_allAtoms[i] = xcb_intern_atom_reply(xcb_connection(), cookies[i], 0)->atom;
}
