#include "qandroidinput.h"

#include <QMutexLocker>
#include <QDebug>
#include <QTimer>

#include <private/qapplication_p.h>


QAndroidInput * QAndroidInput::m_androidInput=NULL;

QAndroidInput::QAndroidInput(QObject *parent)
    : QObject(parent)
{
    mTimer.setInterval(0);
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(consumeEvents()));
}

void QAndroidInput::registerWindow(QWidget * window)
{
    mWindows.push_front(window);
}

void QAndroidInput::unregisterWindow(QWidget * window)
{
    mWindows.remove(mWindows.indexOf(window));
}

void QAndroidInput::addMouseEvent(QMouseEvent * event)
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);
    mMouseEvents.enqueue(event);
    if (!mTimer.isActive())
        mTimer.start();
}

void QAndroidInput::addKeyEvent(QKeyEvent * event)
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);
    mKeyEvents.enqueue(event);
    if (!mTimer.isActive())
        mTimer.start();
}

void QAndroidInput::consumeEvents()
{
    QMutexLocker lock(&mMutex);
    Q_UNUSED(lock);
    while(!mKeyEvents.isEmpty())
    {
        QKeyEvent * event=mKeyEvents.dequeue();
        //QApplicationPrivate::handleKeyEvent(0, event);
        delete event;
    }

    while(!mMouseEvents.isEmpty())
    {
        QMouseEvent * event=mMouseEvents.dequeue();
        //QApplicationPrivate::handleMouseEvent(0, *event);
        delete event;
    }
}
