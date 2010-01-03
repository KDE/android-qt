#include "qandroidinput.h"

#include <QMutexLocker>
#include <QDebug>
#include <QTimer>

#include <private/qapplication_p.h>


QAndroidInput * QAndroidInput::m_androidInput=NULL;

QAndroidInput::QAndroidInput(QObject *parent)
    : QObject(parent)
{
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(consumeEvents()));
    mTimer.setSingleShot(true);
    mTimer.setInterval(1);
}

void QAndroidInput::registerWindow(long mWindowId, QWidget * window)
{
    mMutex.lock();
    mWindows[mWindowId]=window;
    mMutex.unlock();
}

void QAndroidInput::unregisterWindow(long mWindowId)
{
    mMutex.lock();
    mWindows.remove(mWindowId);
    mMutex.unlock();
}

void QAndroidInput::addMouseEvent(long mWindowId, const QMouseEvent & event)
{
    QMutexLocker locker(&mMutex);
    Q_UNUSED(locker);
    QWidget* window=mWindows.value(mWindowId,0);
    if (!window)
        return;
    mMouseEvents.enqueue(QPair<QWidget*, QMouseEvent>(window, event));
    mTimer.start();
}

void QAndroidInput::addKeyEvent(long mWindowId, const QKeyEvent & event)
{
    QMutexLocker locker(&mMutex);
    Q_UNUSED(locker);
    QWidget* window=mWindows.value(mWindowId,0);
    if (!window)
        return;
    mKeyEvents.enqueue(QPair<QWidget*, QKeyEvent>(window, event));
    mTimer.start();
}

void QAndroidInput::consumeEvents()
{
    QMutexLocker locker(&mMutex);
    Q_UNUSED(locker);
    while(!mMouseEvents.isEmpty())
    {
        QPair<QWidget*, QMouseEvent> event=mMouseEvents.dequeue();
        QApplicationPrivate::handleMouseEvent(event.first,event.second);
    }

    while(!mKeyEvents.isEmpty())
    {
        QPair<QWidget*, QKeyEvent> event=mKeyEvents.dequeue();
        QApplicationPrivate::handleKeyEvent(event.first,&event.second);
    }
}
