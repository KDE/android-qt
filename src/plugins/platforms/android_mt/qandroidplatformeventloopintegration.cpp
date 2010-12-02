#include <QCoreApplication>
#include <QThread>

#include "qandroidplatformeventloopintegration.h"
#include "androidjnimain.h"

void QAndroidPlatformEventLoopIntegration::startEventLoop()
{
    // this function will be called from main method thread.
    m_mainMethodThread = QThread::currentThread();
    qApp->moveToThread(QtAndroid::uiThread()); // trigger migration of QApplication to UI thread.
    m_mainMethodSemaphore.acquire();// block the main method until QApplication will quit
}

void QAndroidPlatformEventLoopIntegration::quitEventLoop()
{
    // this function will be called from ui thread (this is the thread where the QApplication is moved).
    qApp->moveToThread(m_mainMethodThread);
    m_mainMethodSemaphore.release();
}

void QAndroidPlatformEventLoopIntegration::qtNeedsToProcessEvents()
{
    QtAndroid::processEvents(nextTimerEvent());
}
