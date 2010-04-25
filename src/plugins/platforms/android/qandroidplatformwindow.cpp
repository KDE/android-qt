#include "androidjnimain.h"
#include "qandroidplatformwindow.h"
#include "qandroidplatformscreen.h"
#include <QWidget>
#include <QDebug>

QT_BEGIN_NAMESPACE

QAndroidPlatformWindow::QAndroidPlatformWindow(QWidget *tlw, QAndroidPlatformScreen * screen):
        QPlatformWindow(tlw)
{
    static QAtomicInt winIdGenerator(1);
    m_windowId = winIdGenerator.fetchAndAddRelaxed(1);
    connect(screen, SIGNAL(screenResized(const QRect &)), SLOT(screenResized(const QRect &)) );
    connect(screen, SIGNAL(update()), tlw, SLOT(update()) );
    m_window = tlw;
    qDebug()<<"QAndroidPlatformWindow::QAndroidPlatformWindow"<<tlw;
}

void QAndroidPlatformWindow::setVisible(bool visible)
{
    QtAndroid::setSurfaceVisiblity(winId(), visible);
}

void QAndroidPlatformWindow::setOpacity(qreal level)
{
    QtAndroid::setSurfaceOpacity(winId(), level);
}

//Qt::WindowFlags QAndroidPlatformWindow::setWindowFlags(Qt::WindowFlags flags)
//{
//    if (flags != Qt::Widget)
//        m_window->setWindowState(m_window->windowState() | Qt::WindowMinimized);
//    return flags;
//}

void QAndroidPlatformWindow::setWindowTitle(const QString & title)
{
    QtAndroid::setWindowTitle(winId(), title);
}

void QAndroidPlatformWindow::raise()
{
    QtAndroid::raiseSurface(winId());
}

void QAndroidPlatformWindow::screenResized(const QRect & rect)
{
    Q_UNUSED(rect)
}

QT_END_NAMESPACE
