#include "androidjnimain.h"
#include "qandroidplatformwindow.h"
#include "qandroidplatformscreen.h"
#include <QWidget>
#include <QDebug>

QT_BEGIN_NAMESPACE

QAndroidPlatformWindow::QAndroidPlatformWindow(QWidget *tlw):
        QPlatformWindow(tlw)
{
    static QAtomicInt winIdGenerator(1);
    m_windowId = winIdGenerator.fetchAndAddRelaxed(1);
    m_window = tlw;
}

void QAndroidPlatformWindow::setVisible(bool visible)
{
    QtAndroid::setSurfaceVisiblity(winId(), visible);
}

void QAndroidPlatformWindow::setOpacity(qreal level)
{
    QtAndroid::setSurfaceOpacity(winId(), level);
}

Qt::WindowFlags QAndroidPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
{
    if (flags != Qt::Widget)
        m_window->setWindowState(m_window->windowState() | Qt::WindowMaximized);
    return flags;
}

void QAndroidPlatformWindow::setWindowTitle(const QString & title)
{
    QtAndroid::setWindowTitle(winId(), title);
}

void QAndroidPlatformWindow::raise()
{
    QtAndroid::raiseSurface(winId());
}

QT_END_NAMESPACE
