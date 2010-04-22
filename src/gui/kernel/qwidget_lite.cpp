/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "QtGui/qwidget.h"
#include "QtGui/qevent.h"
#include "QtGui/qapplication.h"
#include "QtGui/private/qbackingstore_p.h"
#include "QtGui/private/qwidget_p.h"
#include "QtGui/private/qgraphicssystem_p.h"
#include "QtGui/private/qapplication_p.h"
#include "QtGui/qdesktopwidget.h"
#include "QtGui/qplatformwindow_lite.h"

#include <QtGui/QGraphicsSystemCursor>

QT_BEGIN_NAMESPACE
static QPlatformScreen *qt_screenForWidget(const QWidget *w);

void QWidgetPrivate::create_sys(WId window, bool initializeWindow, bool destroyOldWindow)
{
    Q_Q(QWidget);

    Q_UNUSED(window);
    Q_UNUSED(initializeWindow);
    Q_UNUSED(destroyOldWindow);
    // XXX

    Qt::WindowFlags flags = data.window_flags;

    if (!(flags & Qt::Window) || q->windowType() == Qt::Desktop)
        return; // we only care about real toplevels

    QWindowSurface *surface = q->windowSurface();
    QPlatformWindow *platformWindow = q->platformWindow();

    if (!platformWindow) {
        platformWindow = QApplicationPrivate::platformIntegration()->createPlatformWindow(q);
    }
    Q_ASSERT(platformWindow);

    if (!surface) {
        surface = QApplicationPrivate::platformIntegration()->createWindowSurface(q,platformWindow->winId());
    }
    Q_ASSERT(surface);

    data.window_flags = q->platformWindow()->setWindowFlags(data.window_flags);

    setWinId(q->platformWindow()->winId());

//    qDebug() << "create_sys" << q << q->internalWinId();
}

void QWidget::destroy(bool destroyWindow, bool destroySubWindows)
{
    Q_D(QWidget);
    Q_UNUSED(destroySubWindows);

    if ((windowType() == Qt::Popup))
        qApp->d_func()->closePopup(this);

    //### we don't have proper focus event handling yet
    if (this == QApplicationPrivate::active_window)
        QApplication::setActiveWindow(0);

    if (windowType() != Qt::Desktop) {
        if (destroyWindow && isWindow()) {
            QTLWExtra *topData = d->maybeTopData();
            if (topData) {
                delete topData->platformWindow;
                topData->platformWindow = 0;
            }
        } else {
            if (parentWidget() && parentWidget()->testAttribute(Qt::WA_WState_Created)) {
                d->hide_sys();
            }
        }
    }
}

void QWidgetPrivate::setParent_sys(QWidget *newparent, Qt::WindowFlags f)
{
    Q_Q(QWidget);

//    QWidget *oldParent = q->parentWidget();
    Qt::WindowFlags oldFlags = data.window_flags;
    if (parent != newparent) {
        QObjectPrivate::setParent_helper(newparent); //### why does this have to be done in the _sys function???

    }
    if (!newparent) {
        f |= Qt::Window;
    }

    bool explicitlyHidden = q->testAttribute(Qt::WA_WState_Hidden) && q->testAttribute(Qt::WA_WState_ExplicitShowHide);

    data.window_flags = f;
    q->setAttribute(Qt::WA_WState_Created, false);
    q->setAttribute(Qt::WA_WState_Visible, false);
    q->setAttribute(Qt::WA_WState_Hidden, false);

    if (f & Qt::Window) {
        //qDebug() << "setParent_sys" << q << newparent << hex << f;
        if (QPlatformWindow *window = q->platformWindow())
            data.window_flags = window->setWindowFlags(data.window_flags);
    }
    // XXX Reparenting child to toplevel or vice versa ###
    if ((f&Qt::Window) && !(oldFlags&Qt::Window)) {
        //qDebug() << "setParent_sys() change to toplevel";
        q->create(); //### this cannot be right
    } else if ((f&Qt::Window) && !(oldFlags&Qt::Window)) {
        qDebug() << "######## setParent_sys() change from toplevel not implemented ########";
    }

    if (q->isWindow() || (!newparent || newparent->isVisible()) || explicitlyHidden)
        q->setAttribute(Qt::WA_WState_Hidden);
    q->setAttribute(Qt::WA_WState_ExplicitShowHide, explicitlyHidden);

}

QPoint QWidget::mapToGlobal(const QPoint &pos) const
{
    int           x=pos.x(), y=pos.y();
    const QWidget* w = this;
    while (w) {
        x += w->data->crect.x();
        y += w->data->crect.y();
        w = w->isWindow() ? 0 : w->parentWidget();
    }
    return QPoint(x, y);
}

QPoint QWidget::mapFromGlobal(const QPoint &pos) const
{
    int           x=pos.x(), y=pos.y();
    const QWidget* w = this;
    while (w) {
        x -= w->data->crect.x();
        y -= w->data->crect.y();
        w = w->isWindow() ? 0 : w->parentWidget();
    }
    return QPoint(x, y);
}

void QWidgetPrivate::updateSystemBackground() {}

#ifndef QT_NO_CURSOR
void QWidgetPrivate::setCursor_sys(const QCursor &cursor)
{
    Q_UNUSED(cursor);
    Q_Q(QWidget);
    if (q->isVisible())
        qt_lite_set_cursor(q, false);
}

void QWidgetPrivate::unsetCursor_sys()
{
    Q_Q(QWidget);
    if (q->isVisible())
        qt_lite_set_cursor(q, false);
}

void QWidgetPrivate::updateCursor() const
{
    // XXX
}

#endif //QT_NO_CURSOR

void QWidgetPrivate::setWindowTitle_sys(const QString &caption)
{
    Q_Q(QWidget);
    if (!q->isWindow())
        return;

    if (QPlatformWindow *window = q->platformWindow())
        window->setWindowTitle(caption);

}

void QWidgetPrivate::setWindowIcon_sys(bool /*forceReset*/)
{
}

void QWidgetPrivate::setWindowIconText_sys(const QString &iconText)
{
    Q_UNUSED(iconText);
}

QWidget *qt_pressGrab = 0;
QWidget *qt_mouseGrb = 0;
static QWidget *keyboardGrb = 0;

void QWidget::grabMouse()
{
    if (qt_mouseGrb)
        qt_mouseGrb->releaseMouse();

    // XXX
    //qwsDisplay()->grabMouse(this,true);

    qt_mouseGrb = this;
    qt_pressGrab = 0;
}

#ifndef QT_NO_CURSOR
void QWidget::grabMouse(const QCursor &cursor)
{
    Q_UNUSED(cursor);

    if (qt_mouseGrb)
        qt_mouseGrb->releaseMouse();

    // XXX
    //qwsDisplay()->grabMouse(this,true);
    //qwsDisplay()->selectCursor(this, cursor.handle());
    qt_mouseGrb = this;
    qt_pressGrab = 0;
}
#endif

void QWidget::releaseMouse()
{
    if (qt_mouseGrb == this) {
        // XXX
        //qwsDisplay()->grabMouse(this,false);
        qt_mouseGrb = 0;
    }
}

void QWidget::grabKeyboard()
{
    if (keyboardGrb)
        keyboardGrb->releaseKeyboard();
    // XXX
    //qwsDisplay()->grabKeyboard(this, true);
    keyboardGrb = this;
}

void QWidget::releaseKeyboard()
{
    if (keyboardGrb == this) {
        // XXX
        //qwsDisplay()->grabKeyboard(this, false);
        keyboardGrb = 0;
    }
}

QWidget *QWidget::mouseGrabber()
{
    if (qt_mouseGrb)
        return qt_mouseGrb;
    return qt_pressGrab;
}

QWidget *QWidget::keyboardGrabber()
{
    return keyboardGrb;
}

void QWidget::activateWindow()
{
    // XXX
//    qDebug() << "QWidget::activateWindow" << this;
    QApplication::setActiveWindow(this); //#####
}

void QWidgetPrivate::show_sys()
{
    Q_Q(QWidget);
    q->setAttribute(Qt::WA_Mapped);
    if (q->testAttribute(Qt::WA_DontShowOnScreen)) {
        invalidateBuffer(q->rect());
        return;
    }

    QApplication::postEvent(q, new QUpdateLaterEvent(q->rect()));

    if (!q->isWindow())
        return;

    if (QWindowSurface *surface = q->windowSurface()) {
         const QRect geomRect = q->geometry();
         const QRect windowRect = q->platformWindow()->geometry();
         if (windowRect != geomRect) {
             q->platformWindow()->setGeometry(geomRect);
             if (windowRect.size() != geomRect.size()) {
                surface->resize(geomRect.size());
             }
         }
         q->platformWindow()->setVisible(true);
     }

    if (q->windowType() != Qt::Popup && q->windowType() != Qt::ToolTip && !(q->windowFlags() & Qt::X11BypassWindowManagerHint))
        q->activateWindow(); //###
}


void QWidgetPrivate::hide_sys()
{
    Q_Q(QWidget);
    q->setAttribute(Qt::WA_Mapped, false);
    if (!q->isWindow()) {
        QWidget *p = q->parentWidget();
        if (p &&p->isVisible()) {
            invalidateBuffer(q->rect());
        }
        return;
    }
    if (QPlatformWindow *window = q->platformWindow()) {
         window->setVisible(false);
     }

    //### we don't yet have proper focus event handling
    if (q == QApplicationPrivate::active_window)
        QApplication::setActiveWindow(0);

}

void QWidgetPrivate::setMaxWindowState_helper()
{
    setFullScreenSize_helper(); //### decoration size
}

void QWidgetPrivate::setFullScreenSize_helper()
{
    Q_Q(QWidget);

    const uint old_state = data.in_set_window_state;
    data.in_set_window_state = 1;

    const QRect screen = qApp->desktop()->screenGeometry(qApp->desktop()->screenNumber(q));
    q->move(screen.topLeft());
    q->resize(screen.size());

    data.in_set_window_state = old_state;
}

static Qt::WindowStates effectiveState(Qt::WindowStates state)
 {
     if (state & Qt::WindowMinimized)
         return Qt::WindowMinimized;
     else if (state & Qt::WindowFullScreen)
         return Qt::WindowFullScreen;
     else if (state & Qt::WindowMaximized)
         return Qt::WindowMaximized;
     return Qt::WindowNoState;
 }

void QWidget::setWindowState(Qt::WindowStates newstate)
{
    Q_D(QWidget);
    Qt::WindowStates oldstate = windowState();
    if (oldstate == newstate)
        return;
    if (isWindow() && !testAttribute(Qt::WA_WState_Created))
        create();

    data->window_state = newstate;
    data->in_set_window_state = 1;
    bool needShow = false;
    Qt::WindowStates newEffectiveState = effectiveState(newstate);
    Qt::WindowStates oldEffectiveState = effectiveState(oldstate);
    if (isWindow() && newEffectiveState != oldEffectiveState) {
        d->createTLExtra();
        if (oldEffectiveState == Qt::WindowNoState) { //normal
            d->topData()->normalGeometry = geometry();
        } else if (oldEffectiveState == Qt::WindowFullScreen) {
            setParent(0, d->topData()->savedFlags);
            needShow = true;
        } else if (oldEffectiveState == Qt::WindowMinimized) {
            needShow = true;
        }

        if (newEffectiveState == Qt::WindowMinimized) {
            //### not ideal...
            hide();
            needShow = false;
        } else if (newEffectiveState == Qt::WindowFullScreen) {
            d->topData()->savedFlags = windowFlags();
            setParent(0, Qt::FramelessWindowHint | (windowFlags() & Qt::WindowStaysOnTopHint));
            d->setFullScreenSize_helper();
            raise();
            needShow = true;
        } else if (newEffectiveState == Qt::WindowMaximized) {
            createWinId();
            d->setMaxWindowState_helper();
        } else { //normal
            QRect r = d->topData()->normalGeometry;
            if (r.width() >= 0) {
                d->topData()->normalGeometry = QRect(0,0,-1,-1);
                setGeometry(r);
            }
        }
    }
    data->in_set_window_state = 0;

    if (needShow)
        show();

    if (newstate & Qt::WindowActive)
        activateWindow();

    QWindowStateChangeEvent e(oldstate);
    QApplication::sendEvent(this, &e);
}

void QWidgetPrivate::setFocus_sys()
{

}

void QWidgetPrivate::raise_sys()
{
    Q_Q(QWidget);
    if (q->isWindow()) {
        q->platformWindow()->raise();
    }
}

void QWidgetPrivate::lower_sys()
{
    Q_Q(QWidget);
    if (q->isWindow()) {
        Q_ASSERT(q->testAttribute(Qt::WA_WState_Created));
        q->platformWindow()->lower();
    } else if (QWidget *p = q->parentWidget()) {
        setDirtyOpaqueRegion();
        p->d_func()->invalidateBuffer(effectiveRectFor(q->geometry()));
    }
}

void QWidgetPrivate::stackUnder_sys(QWidget*)
{
    Q_Q(QWidget);
    if (QWidget *p = q->parentWidget()) {
        setDirtyOpaqueRegion();
        p->d_func()->invalidateBuffer(effectiveRectFor(q->geometry()));
    }
}

void QWidgetPrivate::setGeometry_sys(int x, int y, int w, int h, bool isMove)
{
    Q_Q(QWidget);
    if (extra) {                                // any size restrictions?
        w = qMin(w,extra->maxw);
        h = qMin(h,extra->maxh);
        w = qMax(w,extra->minw);
        h = qMax(h,extra->minh);
    }

    QPoint oldp = q->geometry().topLeft();
    QSize olds = q->size();
    QRect r(x, y, w, h);

    bool isResize = olds != r.size();
    isMove = oldp != r.topLeft(); //### why do we have isMove as a parameter?


    // We only care about stuff that changes the geometry, or may
    // cause the window manager to change its state
    if (r.size() == olds && oldp == r.topLeft())
        return;

    if (!data.in_set_window_state) {
        q->data->window_state &= ~Qt::WindowMaximized;
        q->data->window_state &= ~Qt::WindowFullScreen;
        if (q->isWindow())
            topData()->normalGeometry = QRect(0, 0, -1, -1);
    }

    QPoint oldPos = q->pos();
    data.crect = r;

    if (q->isVisible()) {

        if (q->isWindow()) {
            const QWidgetBackingStore *bs = maybeBackingStore();
            if (bs->windowSurface) {
                q->platformWindow()->setGeometry(q->frameGeometry());
                if (isResize)
                    bs->windowSurface->resize(r.size());
            }
        } else {
            if (isMove && !isResize)
                moveRect(QRect(oldPos, olds), x - oldPos.x(), y - oldPos.y());
            else
                invalidateBuffer_resizeHelper(oldPos, olds);
        }

        if (isMove) {
            QMoveEvent e(q->pos(), oldPos);
            QApplication::sendEvent(q, &e);
        }
        if (isResize) {
            QResizeEvent e(r.size(), olds);
            QApplication::sendEvent(q, &e);
            if (q->isWindow())
                q->update();
        }
    } else { // not visible
        if (isMove && q->pos() != oldPos)
            q->setAttribute(Qt::WA_PendingMoveEvent, true);
        if (isResize)
            q->setAttribute(Qt::WA_PendingResizeEvent, true);
    }

}

void QWidgetPrivate::setConstraints_sys()
{
}

void QWidgetPrivate::scroll_sys(int dx, int dy)
{
    Q_Q(QWidget);
    scrollChildren(dx, dy);
    scrollRect(q->rect(), dx, dy);
}

void QWidgetPrivate::scroll_sys(int dx, int dy, const QRect &r)
{
    scrollRect(r, dx, dy);
}

static QPlatformScreen *qt_screenForWidget(const QWidget *w)
{
    if (!w)
        return 0;
    QRect frame = w->frameGeometry();
    if (!w->isWindow())
        frame.moveTopLeft(w->mapToGlobal(QPoint(0, 0)));
    const QPoint p = (frame.topLeft() + frame.bottomRight()) / 2;

    QPlatformIntegration *pi = QApplicationPrivate::platformIntegration();
    QList<QPlatformScreen *> screens = pi->screens();

    for (int i = 0; i < screens.size(); ++i) {
        if (screens[i]->geometry().contains(p))
            return screens[i];
    }

    // Assume screen zero if we have it.
    if (!screens.isEmpty())
        return screens[0];
    else
        qWarning("qt_screenForWidget: no screens");

    return 0;
}

int QWidget::metric(PaintDeviceMetric m) const
{
    Q_D(const QWidget);

    QPlatformScreen *screen = qt_screenForWidget(this);
    if (!screen) {
        if (m == PdmDpiX || m == PdmDpiY)
              return 72;
        return QPaintDevice::metric(m);
    }
    int val;
    if (m == PdmWidth) {
        val = data->crect.width();
    } else if (m == PdmWidthMM) {
        val = data->crect.width() * screen->physicalSize().width() / screen->geometry().width();
    } else if (m == PdmHeight) {
        val = data->crect.height();
    } else if (m == PdmHeightMM) {
        val = data->crect.height() * screen->physicalSize().height() / screen->geometry().height();
    } else if (m == PdmDepth) {
        return screen->depth();
    } else if (m == PdmDpiX || m == PdmPhysicalDpiX) {
        if (d->extra && d->extra->customDpiX)
            return d->extra->customDpiX;
        else if (d->parent)
            return static_cast<QWidget *>(d->parent)->metric(m);
        return qRound(screen->geometry().width() / double(screen->physicalSize().width() / 25.4));
    } else if (m == PdmDpiY || m == PdmPhysicalDpiY) {
        if (d->extra && d->extra->customDpiY)
            return d->extra->customDpiY;
        else if (d->parent)
            return static_cast<QWidget *>(d->parent)->metric(m);
        return qRound(screen->geometry().height() / double(screen->physicalSize().height() / 25.4));
    } else {
        val = QPaintDevice::metric(m);// XXX
    }
    return val;
}

void QWidgetPrivate::createSysExtra()
{
}

void QWidgetPrivate::deleteSysExtra()
{
}

void QWidgetPrivate::createTLSysExtra()
{
}

void QWidgetPrivate::deleteTLSysExtra()
{
}

void QWidgetPrivate::registerDropSite(bool on)
{
    Q_UNUSED(on);
}

void QWidgetPrivate::setMask_sys(const QRegion &region)
{
    Q_UNUSED(region);
    // XXX
}

void QWidgetPrivate::updateFrameStrut()
{
    // XXX
}

void QWidgetPrivate::setWindowOpacity_sys(qreal level)
{
    Q_Q(QWidget);
    q->platformWindow()->setOpacity(level);
}

void QWidgetPrivate::setWSGeometry(bool dontShow, const QRect &oldRect)
{
    Q_UNUSED(dontShow);
    Q_UNUSED(oldRect);
    // XXX
}

QPaintEngine *QWidget::paintEngine() const
{
    qWarning("QWidget::paintEngine: Should no longer be called");
    return 0; //##### @@@
}

QWindowSurface *QWidgetPrivate::createDefaultWindowSurface_sys()
{
    qFatal("CreateDefaultWindowSurface_sys should not be used on lighthouse");
    return 0;
}

void QWidgetPrivate::setModal_sys()
{
}

void qt_lite_set_cursor(QWidget * w, bool force)
{
    static QPointer<QWidget> lastUnderMouse = 0;

    QCursor * override = QApplication::overrideCursor();

    if (override && w != 0)
        return;

    QPointer<QGraphicsSystemCursor> cursor = QGraphicsSystemCursor::getInstance();
    if (!cursor)
        return;

    if (w == 0) {
        if (override) {
            cursor->changeCursor(override, QApplication::topLevelAt(QCursor::pos()));
            return;
        }
        w = QApplication::widgetAt(QCursor::pos());
        if (w == 0) // clear the override cursor while over empty space
            w = QApplication::desktop();
    } else if (force) {
        lastUnderMouse = w;
    } else if (w->testAttribute(Qt::WA_WState_Created) && lastUnderMouse
               && lastUnderMouse->effectiveWinId() == w->effectiveWinId()) {
        w = lastUnderMouse;
    }

    if (w == QApplication::desktop() && !override) {
        QCursor c(Qt::ArrowCursor);
        cursor->changeCursor(&c, w);
        return;
    }

    QWidget * curWin = QApplication::activeWindow();
    if (!curWin && w && w->internalWinId())
        return;
    QWidget* cW = w && !w->internalWinId() ? w : curWin;

    if (!cW || cW->window() != w->window() ||
         !cW->isVisible() || !cW->underMouse() || override)
        return;

    QCursor c = w->cursor();
    cursor->changeCursor(&c, w);
}
QT_END_NAMESPACE
