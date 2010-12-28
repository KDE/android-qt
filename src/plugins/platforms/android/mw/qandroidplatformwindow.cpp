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
    QtAndroid::createWindow(false, tlw, m_windowId, m_window->geometry().left(), m_window->geometry().top(),
                                            m_window->geometry().right(), m_window->geometry().bottom());
}

QAndroidPlatformWindow::~QAndroidPlatformWindow()
{
    QtAndroid::destroyWindow(m_windowId);
}

void QAndroidPlatformWindow::setGeometry(const QRect &rect)
{
    m_window->setGeometry(rect);
    QtAndroid::resizeWindow(m_windowId, rect.left(), rect.top(),
                                            rect.right(), rect.bottom());
}


void QAndroidPlatformWindow::setVisible(bool visible)
{
    QtAndroid::setWindowVisiblity(winId(), visible);
}


void QAndroidPlatformWindow::setOpacity(qreal level)
{
    QtAndroid::setWindowOpacity(winId(), level);
}

Qt::WindowFlags QAndroidPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
{
   qDebug()<<"QAndroidPlatformWindow::setWindowFlags"<<flags;
   if (flags != Qt::Popup && flags != Qt::Tool && flags != Qt::ToolTip && flags != Qt::SplashScreen)
        m_window->setWindowState(m_window->windowState() | Qt::WindowMaximized);
    return flags;
}

void QAndroidPlatformWindow::setWindowTitle(const QString & title)
{
    QtAndroid::setWindowTitle(winId(), title);
}

void QAndroidPlatformWindow::raise()
{
    QtAndroid::raiseWindow(winId());
}

void QAndroidPlatformWindow::lower()
{
    QtAndroid::lowerWindow(winId());
}

QT_END_NAMESPACE
