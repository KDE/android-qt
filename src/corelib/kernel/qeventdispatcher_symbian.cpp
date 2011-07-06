/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qeventdispatcher_symbian_p.h"
#include <private/qthread_p.h>
#include <qcoreapplication.h>
#include <private/qcoreapplication_p.h>
#include <qsemaphore.h>

#include <unistd.h>
#include <errno.h>

QT_BEGIN_NAMESPACE

#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
// when the system UI is Qt based, priority drop is not needed as CPU starved processes will not be killed.
#undef QT_SYMBIAN_PRIORITY_DROP
#else
#define QT_SYMBIAN_PRIORITY_DROP
#endif

#define WAKE_UP_PRIORITY CActive::EPriorityStandard
#define TIMER_PRIORITY CActive::EPriorityHigh

static inline int qt_pipe_write(int socket, const char *data, qint64 len)
{
	return ::write(socket, data, len);
}
#if defined(write)
# undef write
#endif

static inline int qt_pipe_close(int socket)
{
	return ::close(socket);
}
#if defined(close)
# undef close
#endif

static inline int qt_pipe_fcntl(int socket, int command)
{
	return ::fcntl(socket, command);
}
static inline int qt_pipe2_fcntl(int socket, int command, int option)
{
	return ::fcntl(socket, command, option);
}
#if defined(fcntl)
# undef fcntl
#endif

static inline int qt_socket_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	return ::select(nfds, readfds, writefds, exceptfds, timeout);
}

// This simply interrupts the select and locks the mutex until destroyed.
class QSelectMutexGrabber
{
public:
    QSelectMutexGrabber(int writeFd, int readFd, QMutex *mutex)
        : m_mutex(mutex)
    {
        if (m_mutex->tryLock())
            return;

        char dummy = 0;
        qt_pipe_write(writeFd, &dummy, 1);

        m_mutex->lock();

        char buffer;
        while (::read(readFd, &buffer, 1) > 0) {}
    }

    ~QSelectMutexGrabber()
    {
        m_mutex->unlock();
    }

private:
    QMutex *m_mutex;
};

/*
 * This class can be used as a base class for Qt active objects.
 * Socket active objects can use it to defer their activity.
 */
QActiveObject::QActiveObject(TInt priority, QEventDispatcherSymbian *dispatcher)
    : CActive(priority),
      m_dispatcher(dispatcher)
{
}

QActiveObject::~QActiveObject()
{
}

bool QActiveObject::maybeDeferSocketEvent()
{
    Q_ASSERT(m_dispatcher);
    if (!m_dispatcher->areSocketEventsBlocked()) {
        return false;
    }
    m_dispatcher->addDeferredSocketActiveObject(this);
    return true;
}

void QActiveObject::reactivateAndComplete()
{
    TInt error = iStatus.Int();
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus *status = &iStatus;
    QEventDispatcherSymbian::RequestComplete(status, error);
}

QWakeUpActiveObject::QWakeUpActiveObject(QEventDispatcherSymbian *dispatcher)
    : CActive(WAKE_UP_PRIORITY),
      m_dispatcher(dispatcher)
{
    m_hostThreadId = RThread().Id();
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
}

QWakeUpActiveObject::~QWakeUpActiveObject()
{
    Cancel();
}

void QWakeUpActiveObject::DoCancel()
{
    if (iStatus.Int() == KRequestPending) {
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    } else if (IsActive() && m_hostThreadId != RThread().Id()) {
        // This is being cancelled in the adopted monitor thread, which can happen if an adopted thread with
        // an event loop has exited. The event loop creates an event dispatcher with this active object, which may be complete but not run on exit.
        // We force a cancellation in this thread, because a) the object cannot be deleted while active and b) without a cancellation
        // the thread semaphore will be one count down.
        // It is possible for this problem to affect other active objects. They symptom would be that finished signals
        // from adopted threads are not sent, or they arrive much later than they should.
        TRequestStatus *status = &iStatus;
        User::RequestComplete(status, KErrNone);
    }
}

void QWakeUpActiveObject::RunL()
{
    iStatus = KRequestPending;
    SetActive();
    QT_TRYCATCH_LEAVING(m_dispatcher->wakeUpWasCalled());
}

QTimerActiveObject::QTimerActiveObject(QEventDispatcherSymbian *dispatcher, SymbianTimerInfo *timerInfo)
    : CActive(TIMER_PRIORITY),
      m_dispatcher(dispatcher), m_timerInfo(timerInfo), m_expectedTimeSinceLastEvent(0)
{
    // start the timeout timer to ensure initialisation
    m_timeoutTimer.start();
}

QTimerActiveObject::~QTimerActiveObject()
{
    Cancel();
    m_rTimer.Close(); //close of null handle is safe
}

void QTimerActiveObject::DoCancel()
{
    if (m_timerInfo->interval > 0) {
        m_rTimer.Cancel();
    } else {
        if (iStatus.Int() == KRequestPending) {
            TRequestStatus *status = &iStatus;
            QEventDispatcherSymbian::RequestComplete(status, KErrNone);
        }
    }
}

void QTimerActiveObject::RunL()
{
    int error = KErrNone;
    if (iStatus == KErrNone) {
        QT_TRYCATCH_ERROR(error, Run());
    } else {
        error = iStatus.Int();
    }
    // All Symbian error codes are negative.
    if (error < 0) {
        CActiveScheduler::Current()->Error(error);  // stop and report here, as this timer will be deleted on scope exit
    }
}

#define MAX_SYMBIAN_TIMEOUT_MS 2000000
void QTimerActiveObject::StartTimer()
{
    if (m_timerInfo->msLeft > MAX_SYMBIAN_TIMEOUT_MS) {
        //There is loss of accuracy anyway due to needing to restart the timer every 33 minutes,
        //so the 1/64s res of After() is acceptable for these very long timers.
        m_rTimer.After(iStatus, MAX_SYMBIAN_TIMEOUT_MS * 1000);
        m_timerInfo->msLeft -= MAX_SYMBIAN_TIMEOUT_MS;
    } else {
        // this algorithm implements drift correction for repeating timers
        // calculate how late we are for this event
        int timeSinceLastEvent = m_timeoutTimer.restart();
        int overshoot = timeSinceLastEvent - m_expectedTimeSinceLastEvent;
        if (overshoot > m_timerInfo->msLeft) {
            // we skipped a whole timeout, restart from here
            overshoot = 0;
        }
        // calculate when the next event should happen
        int waitTime = m_timerInfo->msLeft - overshoot;
        m_expectedTimeSinceLastEvent = waitTime;
        // limit the actual ms wait time to avoid wild corrections
        // this will cause the real event time to slowly drift back to the expected event time
        // measurements show that Symbian timers always fire 1 or 2 ms late
        const int limit = 4;
        waitTime = qMax(m_timerInfo->msLeft - limit, waitTime);
        m_rTimer.HighRes(iStatus, waitTime * 1000);
        m_timerInfo->msLeft = 0;
    }
    SetActive();
}

void QTimerActiveObject::Run()
{
    //restart timer immediately, if the timeout has been split because it overflows max for platform.
    if (m_timerInfo->msLeft > 0) {
        StartTimer();
        return;
    }

    if (m_timerInfo->interval > 0) {
        // Start a new timer immediately so that we don't lose time.
        m_timerInfo->msLeft = m_timerInfo->interval;
        StartTimer();

        m_timerInfo->dispatcher->timerFired(m_timerInfo->timerId);
    } else {
        // However, we only complete zero timers after the event has finished,
        // in order to prevent busy looping when doing nested loops.

        // Keep the refpointer around in order to avoid deletion until the end of this function.
        SymbianTimerInfoPtr timerInfoPtr(m_timerInfo);

        m_timerInfo->dispatcher->timerFired(m_timerInfo->timerId);

        iStatus = KRequestPending;
        SetActive();
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QTimerActiveObject::Start()
{
    CActiveScheduler::Add(this);
    m_timerInfo->msLeft = m_timerInfo->interval;
    if (m_timerInfo->interval > 0) {
        if (!m_rTimer.Handle()) {
            qt_symbian_throwIfError(m_rTimer.CreateLocal());
        }
        m_timeoutTimer.start();
        m_expectedTimeSinceLastEvent = 0;
        StartTimer();
    } else {
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

SymbianTimerInfo::SymbianTimerInfo()
    : timerAO(0)
{
}

SymbianTimerInfo::~SymbianTimerInfo()
{
    delete timerAO;
}

QSelectThread::QSelectThread()
    : m_quit(false)
{
    if (::pipe(m_pipeEnds) != 0) {
        qWarning("Select thread was unable to open a pipe, errno: %i", errno);
    } else {
        int flags0 = qt_pipe_fcntl(m_pipeEnds[0], F_GETFL);
        int flags1 = qt_pipe_fcntl(m_pipeEnds[1], F_GETFL);
        // We should check the error code here, but Open C has a bug that returns
        // failure even though the operation was successful.
        qt_pipe2_fcntl(m_pipeEnds[0], F_SETFL, flags0 | O_NONBLOCK);
        qt_pipe2_fcntl(m_pipeEnds[1], F_SETFL, flags1 | O_NONBLOCK);
    }
}

QSelectThread::~QSelectThread()
{
    qt_pipe_close(m_pipeEnds[1]);
    qt_pipe_close(m_pipeEnds[0]);
}

void QSelectThread::run()
{
    Q_D(QThread);

    m_mutex.lock();

    while (!m_quit) {
        fd_set readfds;
        fd_set writefds;
        fd_set exceptionfds;

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptionfds);

        int maxfd = 0;
        maxfd = qMax(maxfd, updateSocketSet(QSocketNotifier::Read, &readfds));
        maxfd = qMax(maxfd, updateSocketSet(QSocketNotifier::Write, &writefds));
        maxfd = qMax(maxfd, updateSocketSet(QSocketNotifier::Exception, &exceptionfds));
        maxfd = qMax(maxfd, m_pipeEnds[0]);
        maxfd++;

        FD_SET(m_pipeEnds[0], &readfds);

        int ret;
        int savedSelectErrno;
        ret = qt_socket_select(maxfd, &readfds, &writefds, &exceptionfds, 0);
        savedSelectErrno = errno;

        if(ret == 0) {
         // do nothing
        } else if (ret < 0) {
            switch (savedSelectErrno) {
            case EBADF:
            case EINVAL:
            case ENOMEM:
            case EFAULT:
                qWarning("::select() returned an error: %i", savedSelectErrno);
                break;
            case ECONNREFUSED:
            case EPIPE:
                qWarning("::select() returned an error: %i (go through sockets)", savedSelectErrno);
                // prepare to go through all sockets
                // mark in fd sets both:
                //     good ones
                //     ones that return -1 in select
                // after loop update notifiers for all of them

                // as we don't have "exception" notifier type
                // we should force monitoring fd_set of this
                // type as well

                // clean @ start
                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                FD_ZERO(&exceptionfds);
                for (QHash<QSocketNotifier *, TRequestStatus *>::const_iterator i = m_AOStatuses.begin();
                        i != m_AOStatuses.end(); ++i) {

                    fd_set onefds;
                    FD_ZERO(&onefds);
                    FD_SET(i.key()->socket(), &onefds);

                    fd_set excfds;
                    FD_ZERO(&excfds);
                    FD_SET(i.key()->socket(), &excfds);

                    maxfd = i.key()->socket() + 1;

                    struct timeval timeout;
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 0;

                    ret = 0;

                    if(i.key()->type() == QSocketNotifier::Read) {
                        ret = ::select(maxfd, &onefds, 0, &excfds, &timeout);
                        if(ret != 0) FD_SET(i.key()->socket(), &readfds);
                    } else if(i.key()->type() == QSocketNotifier::Write) {
                        ret = ::select(maxfd, 0, &onefds, &excfds, &timeout);
                        if(ret != 0) FD_SET(i.key()->socket(), &writefds);
                    }

                } // end for

                // traversed all, so update
                updateActivatedNotifiers(QSocketNotifier::Exception, &exceptionfds);
                updateActivatedNotifiers(QSocketNotifier::Read, &readfds);
                updateActivatedNotifiers(QSocketNotifier::Write, &writefds);

                break;
            case EINTR: // Should never occur on Symbian, but this is future proof!
            default:
                qWarning("::select() returned an unknown error: %i", savedSelectErrno);

                break;
            }
        } else {
            updateActivatedNotifiers(QSocketNotifier::Exception, &exceptionfds);
            updateActivatedNotifiers(QSocketNotifier::Read, &readfds);
            updateActivatedNotifiers(QSocketNotifier::Write, &writefds);
        }

        if (FD_ISSET(m_pipeEnds[0], &readfds))
            m_waitCond.wait(&m_mutex);
    }

    m_mutex.unlock();
}

void QSelectThread::requestSocketEvents ( QSocketNotifier *notifier, TRequestStatus *status )
{
    Q_D(QThread);

    if (!isRunning()) {
        start();
    }

    Q_ASSERT(QThread::currentThread() == this->thread());

    QSelectMutexGrabber lock(m_pipeEnds[1], m_pipeEnds[0], &m_mutex);

    Q_ASSERT(!m_AOStatuses.contains(notifier));

    m_AOStatuses.insert(notifier, status);

    m_waitCond.wakeAll();
}

void QSelectThread::cancelSocketEvents ( QSocketNotifier *notifier )
{
    Q_ASSERT(QThread::currentThread() == this->thread());

    QSelectMutexGrabber lock(m_pipeEnds[1], m_pipeEnds[0], &m_mutex);

    m_AOStatuses.remove(notifier);

    m_waitCond.wakeAll();
}

void QSelectThread::restart()
{
    Q_ASSERT(QThread::currentThread() == this->thread());

    QSelectMutexGrabber lock(m_pipeEnds[1], m_pipeEnds[0], &m_mutex);

    m_waitCond.wakeAll();
}

int QSelectThread::updateSocketSet(QSocketNotifier::Type type, fd_set *fds)
{
    int maxfd = 0;
    if(m_AOStatuses.isEmpty()) {
        /*
         * Wonder if should return -1
         * to signal that no descriptors
         * added to fds
        */
        return maxfd;
    }
    for ( QHash<QSocketNotifier *, TRequestStatus *>::const_iterator i = m_AOStatuses.begin();
            i != m_AOStatuses.end(); ++i) {
        if (i.key()->type() == type) {
            FD_SET(i.key()->socket(), fds);
            maxfd = qMax(maxfd, i.key()->socket());
        } else if(type == QSocketNotifier::Exception) {
            /*
             * We are registering existing sockets
             * always to exception set
             *
             * Doing double FD_SET shouldn't
             * matter
             */
            FD_SET(i.key()->socket(), fds);
            maxfd = qMax(maxfd, i.key()->socket());
        }
    }

    return maxfd;
}

void QSelectThread::updateActivatedNotifiers(QSocketNotifier::Type type, fd_set *fds)
{
    Q_D(QThread);
    if(m_AOStatuses.isEmpty()) {
        return;
    }
    QList<QSocketNotifier *> toRemove;
    for (QHash<QSocketNotifier *, TRequestStatus *>::const_iterator i = m_AOStatuses.begin();
            i != m_AOStatuses.end(); ++i) {
        if (i.key()->type() == type && FD_ISSET(i.key()->socket(), fds)) {
            toRemove.append(i.key());
            TRequestStatus *status = i.value();
            // Thread data is still owned by the main thread.
            QEventDispatcherSymbian::RequestComplete(d->threadData->symbian_thread_handle, status, KErrNone);
        } else if(type == QSocketNotifier::Exception && FD_ISSET(i.key()->socket(), fds)) {
            /*
             * check if socket is in exception set
             * then signal RequestComplete for it
             */
            qWarning("exception on %d [will close the socket handle - hack]", i.key()->socket());
            // quick fix; there is a bug
            // when doing read on socket
            // errors not preoperly mapped
            // after offline-ing the device
            // on some devices we do get exception
            ::close(i.key()->socket());
            toRemove.append(i.key());
            TRequestStatus *status = i.value();
            QEventDispatcherSymbian::RequestComplete(d->threadData->symbian_thread_handle, status, KErrNone);
        }
    }

    for (int c = 0; c < toRemove.size(); ++c) {
        m_AOStatuses.remove(toRemove[c]);
    }
}

void QSelectThread::stop()
{
    m_quit = true;
    restart();
    wait();
}

QSocketActiveObject::QSocketActiveObject(QEventDispatcherSymbian *dispatcher, QSocketNotifier *notifier)
    : QActiveObject(CActive::EPriorityStandard, dispatcher),
      m_notifier(notifier),
      m_inSocketEvent(false),
      m_deleteLater(false)
{
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
}

QSocketActiveObject::~QSocketActiveObject()
{
    Cancel();
}

void QSocketActiveObject::DoCancel()
{
    if (iStatus.Int() == KRequestPending) {
        TRequestStatus *status = &iStatus;
        QEventDispatcherSymbian::RequestComplete(status, KErrNone);
    }
}

void QSocketActiveObject::RunL()
{
    if (maybeDeferSocketEvent())
        return;

    QT_TRYCATCH_LEAVING(run());
}

void QSocketActiveObject::run()
{
    QEvent e(QEvent::SockAct);
    m_inSocketEvent = true;
    QCoreApplication::sendEvent(m_notifier, &e);
    m_inSocketEvent = false;

    if (m_deleteLater) {
        delete this;
    } else {
        iStatus = KRequestPending;
        SetActive();
        m_dispatcher->reactivateSocketNotifier(m_notifier);
    }
}

void QSocketActiveObject::deleteLater()
{
    if (m_inSocketEvent) {
        m_deleteLater = true;
    } else {
        delete this;
    }
}

/* Round robin active object scheduling for Qt apps.
 *
 * Qt and Symbian have different views on how events should be handled. Qt expects
 * round-robin event processing, whereas Symbian implements a strict priority based
 * system.
 *
 * This scheduler class, and its use in QEventDispatcherSymbian::processEvents,
 * introduces round robin scheduling for high priority active objects, but leaves
 * those with low priorities scheduled in priority order.
 * The algorithm used is that, during each call to processEvents, any pre-existing
 * runnable active object may run, but only once. Active objects with priority
 * lower than EPriorityStandard can only run if no higher priority active object
 * has run.
 * This is done by implementing an alternative scheduling algorithm which requires
 * access to the internal members of the active object system. The iSpare member of
 * CActive is replaced with a flag indicating that the object is new (CBase zero
 * initialization sets this), or not run, or ran. Only active objects with the
 * not run flag are allowed to run.
 */
class QtRRActiveScheduler
{
public:
    static void MarkReadyToRun();
    enum RunResult {
        NothingFound,
        ObjectRun,
        ObjectDelayed
    };
    static RunResult RunMarkedIfReady(TInt &runPriority, TInt minimumPriority);
    static bool UseRRActiveScheduler();

private:
    // active scheduler access kit, for gaining access to the internals of active objects for
    // alternative active scheduler implementations.
    class TRequestStatusAccess
    {
    public:
        enum { ERequestActiveFlags = 3 };  // TRequestStatus::EActive | TRequestStatus::ERequestPending
        TInt iStatus;
        TUint iFlags;
    };

    class CActiveDataAccess : public CBase
    {
    public:
        TRequestStatusAccess iStatus;
        TPriQueLink iLink;
        enum TMarks
        {
            ENewObject,     // CBase zero initialization sets this, new objects cannot be run in the processEvents in which they are created
            ENotRun,        // This object has not yet run in the current processEvents call
            ERan            // This object has run in the current processEvents call
        };
        int iMark;      //TAny* iSpare;
    };

    class CActiveFuncAccess : public CActive
    {
    public:
        // these functions are needed in RunMarkedIfReady
        using CActive::RunL;
        using CActive::RunError;
    };

    class CActiveSchedulerAccess : public CBase
    {
    public:
        using CBase::Extension_;
        struct TLoop;
        TLoop* iStack;
        TPriQue<CActiveFuncAccess> iActiveQ;
        TAny* iSpare;
    };
};

void QtRRActiveScheduler::MarkReadyToRun()
{
    CActiveScheduler *pS=CActiveScheduler::Current();
    if (pS!=NULL)
    {
        TDblQueIter<CActive> iterator(((CActiveSchedulerAccess*)pS)->iActiveQ);
        for (CActive* active=iterator++; active!=NULL; active=iterator++) {
            ((CActiveDataAccess*)active)->iMark = CActiveDataAccess::ENotRun;
        }
    }
}

QtRRActiveScheduler::RunResult QtRRActiveScheduler::RunMarkedIfReady(TInt &runPriority, TInt minimumPriority)
{
    RunResult result = NothingFound;
    TInt error=KErrNone;
    CActiveScheduler *pS=CActiveScheduler::Current();
    if (pS!=NULL) {
        TDblQueIter<CActiveFuncAccess> iterator(((CActiveSchedulerAccess*)pS)->iActiveQ);
        for (CActiveFuncAccess *active=iterator++; active!=NULL; active=iterator++) {
            CActiveDataAccess *dataAccess = (CActiveDataAccess*)active;
            if (active->IsActive() && (active->iStatus!=KRequestPending)) {
                int& mark = dataAccess->iMark;
                if (mark == CActiveDataAccess::ENotRun && active->Priority()>=minimumPriority) {
                    mark = CActiveDataAccess::ERan;
                    runPriority = active->Priority();
                    dataAccess->iStatus.iFlags&=~TRequestStatusAccess::ERequestActiveFlags;
                    int vptr = *(int*)active;       // vptr can be used to identify type when debugging leaves
                    TRAP(error, active->RunL());
                    if (error!=KErrNone)
                        error=active->RunError(error);
                    if (error) {
                        qWarning("Active object (ptr=0x%08x, vptr=0x%08x) leave: %i\n", active, vptr, error);
                        pS->Error(error);
                    }
                    return ObjectRun;
                }
                result = ObjectDelayed;
            }
        }
    }
    return result;
}

bool QtRRActiveScheduler::UseRRActiveScheduler()
{
    // This code allows euser to declare incompatible active object / scheduler internal data structures
    // in the future, disabling Qt's round robin scheduler use.
    // By default the Extension_ function will set the second argument to NULL. We therefore use NULL to indicate
    // that the data structures are compatible with before when this protocol was recognised.
    // The extension id used is QtCore's UID.
    CActiveSchedulerAccess *access = (CActiveSchedulerAccess *)CActiveScheduler::Current();
    TAny* schedulerCompatibilityNumber;
    access->Extension_(0x2001B2DC, schedulerCompatibilityNumber, NULL);
    return schedulerCompatibilityNumber == NULL;
}

#ifdef QT_SYMBIAN_PRIORITY_DROP
class QIdleDetectorThread
{
public:
    QIdleDetectorThread()
    : m_state(STATE_RUN), m_stop(false), m_running(false)
    {
        start();
    }

    ~QIdleDetectorThread()
    {
        stop();
    }

    void start()
    {
        QMutexLocker lock(&m_mutex);
        if (m_running)
            return;
        m_stop = false;
        m_state = STATE_RUN;
        TInt err = m_idleDetectorThread.Create(KNullDesC(), &idleDetectorThreadFunc, 1024, &User::Allocator(), this);
        if (err != KErrNone)
            return;     // Fail silently on error. Next kick will try again. Exception might stop the event being processed
        m_idleDetectorThread.SetPriority(EPriorityAbsoluteBackgroundNormal);
        m_idleDetectorThread.Resume();
        m_running = true;
        // get a callback from QCoreApplication destruction to stop this thread
        qAddPostRoutine(StopIdleDetectorThread);
    }

    void stop()
    {
        QMutexLocker lock(&m_mutex);
        if (!m_running)
            return;
        // close down the idle thread because if corelib is loaded temporarily, this would leak threads into the host process
        m_stop = true;
        m_kick.release();
        m_idleDetectorThread.SetPriority(EPriorityNormal);
        TRequestStatus s;
        m_idleDetectorThread.Logon(s);
        User::WaitForRequest(s);
        m_idleDetectorThread.Close();
        m_running = false;
    }

    void kick()
    {
        start();
        m_state = STATE_KICKED;
        m_kick.release();
    }

    bool hasRun()
    {
        return m_state == STATE_RUN;
    }

private:
    static TInt idleDetectorThreadFunc(TAny* self)
    {
        User::RenameThread(_L("IdleDetectorThread"));
        static_cast<QIdleDetectorThread*>(self)->IdleLoop();
        return KErrNone;
    }

    void IdleLoop()
    {
        while (!m_stop) {
            m_kick.acquire();
            m_state = STATE_RUN;
        }
    }

    static void StopIdleDetectorThread();

private:
    enum IdleStates {STATE_KICKED, STATE_RUN} m_state;
    bool m_stop;
    bool m_running;
    RThread m_idleDetectorThread;
    QSemaphore m_kick;
    QMutex m_mutex;
};

Q_GLOBAL_STATIC(QIdleDetectorThread, idleDetectorThread);

void QIdleDetectorThread::StopIdleDetectorThread()
{
    idleDetectorThread()->stop();
}

const int maxBusyTime = 2000; // maximum time we allow idle detector to be blocked before worrying, in milliseconds
const int baseDelay = 1000; // minimum delay time used when backing off to allow idling, in microseconds
#endif

QEventDispatcherSymbian::QEventDispatcherSymbian(QObject *parent)
    : QAbstractEventDispatcher(parent),
      m_selectThread(0),
      m_activeScheduler(0),
      m_wakeUpAO(0),
      m_interrupt(false),
      m_wakeUpDone(0),
      m_iterationCount(0),
      m_insideTimerEvent(false),
      m_noSocketEvents(false)
{
#ifdef QT_SYMBIAN_PRIORITY_DROP
    m_delay = baseDelay;
    m_avgEventTime = 0;
    idleDetectorThread();
#endif
}

QEventDispatcherSymbian::~QEventDispatcherSymbian()
{
}

void QEventDispatcherSymbian::startingUp()
{
    if( !CActiveScheduler::Current() ) {
        m_activeScheduler = q_check_ptr(new CQtActiveScheduler());	// CBase derived class needs to be checked on new
        CActiveScheduler::Install(m_activeScheduler);
    }
    m_wakeUpAO = q_check_ptr(new QWakeUpActiveObject(this));
    // We already might have posted events, wakeup once to process them
    wakeUp();
}

QSelectThread& QEventDispatcherSymbian::selectThread() {
    if (!m_selectThread)
        m_selectThread = new QSelectThread;
    return *m_selectThread;
}

void QEventDispatcherSymbian::closingDown()
{
    if (m_selectThread && m_selectThread->isRunning()) {
        m_selectThread->stop();
    }
    delete m_selectThread;
    m_selectThread = 0;

    delete m_wakeUpAO;
    if (m_activeScheduler) {
        delete m_activeScheduler;
    }
}

bool QEventDispatcherSymbian::processEvents ( QEventLoop::ProcessEventsFlags flags )
{
    const bool useRRScheduler = QtRRActiveScheduler::UseRRActiveScheduler();
    bool handledAnyEvent = false;
    bool oldNoSocketEventsValue = m_noSocketEvents;
    bool oldInsideTimerEventValue = m_insideTimerEvent;

    m_insideTimerEvent = false;

    QT_TRY {
        Q_D(QAbstractEventDispatcher);

        // It is safe if this counter overflows. The main importance is that each
        // iteration count is different from the last.
        m_iterationCount++;

        RThread &thread = d->threadData->symbian_thread_handle;

        bool block;
        if (flags & QEventLoop::WaitForMoreEvents) {
            block = true;
            emit aboutToBlock();
        } else {
            block = false;
        }

        if (flags & QEventLoop::ExcludeSocketNotifiers) {
            m_noSocketEvents = true;
        } else {
            m_noSocketEvents = false;
            handledAnyEvent = sendDeferredSocketEvents();
        }

        QtRRActiveScheduler::RunResult handledSymbianEvent = QtRRActiveScheduler::NothingFound;
        m_interrupt = false;
        int minPriority = KMinTInt;

#ifdef QT_SYMBIAN_PRIORITY_DROP
        QElapsedTimer eventTimer;
#endif

        while (1) {
            if (block) {
                // This is where Qt will spend most of its time.
                CActiveScheduler::Current()->WaitForAnyRequest();
            } else {
                if (thread.RequestCount() == 0) {
#ifdef QT_SYMBIAN_PRIORITY_DROP
                    if (idleDetectorThread()->hasRun()) {
                        m_lastIdleRequestTimer.start();
                        idleDetectorThread()->kick();
                    } else if (m_lastIdleRequestTimer.elapsed() > maxBusyTime) {
                        User::AfterHighRes(m_delay);
                    }
#endif
                    break;
                }
                // This one should return without delay.
                CActiveScheduler::Current()->WaitForAnyRequest();
            }

            if (useRRScheduler && handledSymbianEvent == QtRRActiveScheduler::NothingFound) {
                QtRRActiveScheduler::MarkReadyToRun();
            }

#ifdef QT_SYMBIAN_PRIORITY_DROP
            if (idleDetectorThread()->hasRun()) {
                if (m_delay > baseDelay)
                    m_delay -= baseDelay;
                m_lastIdleRequestTimer.start();
                idleDetectorThread()->kick();
            } else if (m_lastIdleRequestTimer.elapsed() > maxBusyTime) {
                User::AfterHighRes(m_delay);
                // allow delay to be up to 1/4 of execution time
                if (!idleDetectorThread()->hasRun() && m_delay*3 < m_avgEventTime)
                    m_delay += baseDelay;
            }
            eventTimer.start();
#endif

            if (useRRScheduler) {
                // Standard or above priority AOs are scheduled round robin.
                // Lower priority AOs can only run if nothing higher priority has run.
                int runPriority = minPriority;
                handledSymbianEvent = QtRRActiveScheduler::RunMarkedIfReady(runPriority, minPriority);
                minPriority = qMin(runPriority, int(CActive::EPriorityStandard));
            } else {
                TInt error;
                handledSymbianEvent =
                      CActiveScheduler::RunIfReady(error, minPriority)
                    ? QtRRActiveScheduler::ObjectRun
                    : QtRRActiveScheduler::NothingFound;
                if (error) {
                    qWarning("CActiveScheduler::RunIfReady() returned error: %i\n", error);
                    CActiveScheduler::Current()->Error(error);
                }
            }

            if (handledSymbianEvent == QtRRActiveScheduler::NothingFound) {
                // no runnable or delayed active object was found, the signal that caused us to get here must be bad
                qFatal("QEventDispatcherSymbian::processEvents(): Caught Symbian stray signal");
            } else if (handledSymbianEvent == QtRRActiveScheduler::ObjectDelayed) {
                // signal the thread to compensate for the un-handled signal absorbed
                RThread().RequestSignal();
                break;
            }

#ifdef QT_SYMBIAN_PRIORITY_DROP
            int eventDur = eventTimer.elapsed()*1000;
            // average is calcualted as a 5% decaying exponential average
            m_avgEventTime = (m_avgEventTime * 95 + eventDur * 5) / 100;
#endif

            handledAnyEvent = true;

            if (m_interrupt) {
                break;
            }
            block = false;
        };

        emit awake();
    } QT_CATCH (const std::exception& ex) {
#ifndef QT_NO_EXCEPTIONS
        CActiveScheduler::Current()->Error(qt_symbian_exception2Error(ex));
#endif
    }

    m_noSocketEvents = oldNoSocketEventsValue;
    m_insideTimerEvent = oldInsideTimerEventValue;

    return handledAnyEvent;
}

void QEventDispatcherSymbian::timerFired(int timerId)
{
    QHash<int, SymbianTimerInfoPtr>::iterator i = m_timerList.find(timerId);
    if (i == m_timerList.end()) {
        // The timer has been deleted. Ignore this event.
        return;
    }

    SymbianTimerInfoPtr timerInfo = *i;

    // Prevent infinite timer recursion.
    if (timerInfo->inTimerEvent) {
        return;
    }

    timerInfo->inTimerEvent = true;
    bool oldInsideTimerEventValue = m_insideTimerEvent;
    m_insideTimerEvent = true;

    QTimerEvent event(timerInfo->timerId);
    QCoreApplication::sendEvent(timerInfo->receiver, &event);

    m_insideTimerEvent = oldInsideTimerEventValue;
    timerInfo->inTimerEvent = false;

    return;
}

void QEventDispatcherSymbian::wakeUpWasCalled()
{
    // The reactivation should happen in RunL, right before the call to this function.
    // This is because m_wakeUpDone is the "signal" that the object can be completed
    // once more.
    // Also, by dispatching the posted events after resetting m_wakeUpDone, we guarantee
    // that no posted event notification will be lost. If we did it the other way
    // around, it would be possible for another thread to post an event right after
    // the sendPostedEvents was done, but before the object was ready to be completed
    // again. This could deadlock the application if there are no other posted events.
    m_wakeUpDone.fetchAndStoreOrdered(0);
    sendPostedEvents();
}

void QEventDispatcherSymbian::interrupt()
{
    m_interrupt = true;
    wakeUp();
}

void QEventDispatcherSymbian::wakeUp()
{
    Q_D(QAbstractEventDispatcher);

    if (m_wakeUpAO && m_wakeUpDone.testAndSetAcquire(0, 1)) {
        TRequestStatus *status = &m_wakeUpAO->iStatus;
        QEventDispatcherSymbian::RequestComplete(d->threadData->symbian_thread_handle, status, KErrNone);
    }
}

bool QEventDispatcherSymbian::sendPostedEvents()
{
    Q_D(QAbstractEventDispatcher);

    // moveToThread calls this and canWait == true -> Events will never get processed
    // if we check for d->threadData->canWait
    //
    // QCoreApplication::postEvent sets canWait = false, but after the object and events
    // are moved to a new thread, the canWait in new thread is true i.e. not changed to reflect
    // the flag on old thread. That's why events in a new thread will not get processed.
    // This migth be actually bug in moveToThread functionality, but because other platforms
    // do not check canWait in wakeUp (where we essentially are now) - decided to remove it from
    // here as well.

    //if (!d->threadData->canWait) {
        QCoreApplicationPrivate::sendPostedEvents(0, 0, d->threadData);
        return true;
    //}
    //return false;
}

inline void QEventDispatcherSymbian::addDeferredSocketActiveObject(QActiveObject *object)
{
    m_deferredSocketEvents.append(object);
}

bool QEventDispatcherSymbian::sendDeferredSocketEvents()
{
    bool sentAnyEvents = false;
    while (!m_deferredSocketEvents.isEmpty()) {
        sentAnyEvents = true;
        QActiveObject *object = m_deferredSocketEvents.takeFirst();
        object->reactivateAndComplete();
    }

    return sentAnyEvents;
}

void QEventDispatcherSymbian::flush()
{
}

bool QEventDispatcherSymbian::hasPendingEvents()
{
    Q_D(QAbstractEventDispatcher);
    return (d->threadData->symbian_thread_handle.RequestCount() != 0
            || !d->threadData->canWait || !m_deferredSocketEvents.isEmpty());
}

void QEventDispatcherSymbian::registerSocketNotifier ( QSocketNotifier * notifier )
{
    //check socket descriptor is usable
    if (notifier->socket() >= FD_SETSIZE || notifier->socket() < 0) {
        //same warning message as the unix event dispatcher for easy testing
        qWarning("QSocketNotifier: Internal error");
        return;
    }
    //note - this is only for "open C" file descriptors
    //for native sockets, an active object in the symbian socket engine handles this
    QSocketActiveObject *socketAO = new QSocketActiveObject(this, notifier);
    Q_CHECK_PTR(socketAO);
    m_notifiers.insert(notifier, socketAO);
    selectThread().requestSocketEvents(notifier, &socketAO->iStatus);
}

void QEventDispatcherSymbian::unregisterSocketNotifier ( QSocketNotifier * notifier )
{
    //note - this is only for "open C" file descriptors
    //for native sockets, an active object in the symbian socket engine handles this
    if (m_selectThread)
        m_selectThread->cancelSocketEvents(notifier);
    if (m_notifiers.contains(notifier)) {
        QSocketActiveObject *sockObj = *m_notifiers.find(notifier);
        m_deferredSocketEvents.removeAll(sockObj);
        sockObj->deleteLater();
        m_notifiers.remove(notifier);
    }
}

void QEventDispatcherSymbian::reactivateSocketNotifier(QSocketNotifier *notifier)
{
    selectThread().requestSocketEvents(notifier, &m_notifiers[notifier]->iStatus);
}

void QEventDispatcherSymbian::registerTimer ( int timerId, int interval, QObject * object )
{
    if (interval < 0) {
        qWarning("Timer interval < 0");
        interval = 0;
    }

    SymbianTimerInfoPtr timer(new SymbianTimerInfo);
    timer->timerId      = timerId;
    timer->interval     = interval;
    timer->inTimerEvent = false;
    timer->receiver     = object;
    timer->dispatcher   = this;
    timer->timerAO      = q_check_ptr(new QTimerActiveObject(this, timer.data()));
    m_timerList.insert(timerId, timer);

    timer->timerAO->Start();
}

bool QEventDispatcherSymbian::unregisterTimer ( int timerId )
{
    if (!m_timerList.contains(timerId)) {
        return false;
    }

    SymbianTimerInfoPtr timerInfo = m_timerList.take(timerId);

    if (!QObjectPrivate::get(timerInfo->receiver)->inThreadChangeEvent)
        QAbstractEventDispatcherPrivate::releaseTimerId(timerId);

    return true;
}

bool QEventDispatcherSymbian::unregisterTimers ( QObject * object )
{
    if (m_timerList.isEmpty())
        return false;

    bool unregistered = false;
    for (QHash<int, SymbianTimerInfoPtr>::iterator i = m_timerList.begin(); i != m_timerList.end(); ) {
        if ((*i)->receiver == object) {
            i = m_timerList.erase(i);
            unregistered = true;
        } else {
            ++i;
        }
    }

    return unregistered;
}

QList<QEventDispatcherSymbian::TimerInfo> QEventDispatcherSymbian::registeredTimers ( QObject * object ) const
{
    QList<TimerInfo> list;
    for (QHash<int, SymbianTimerInfoPtr>::const_iterator i = m_timerList.begin(); i != m_timerList.end(); ++i) {
        if ((*i)->receiver == object) {
            list.push_back(TimerInfo((*i)->timerId, (*i)->interval));
        }
    }

    return list;
}

/*
 * This active scheduler class implements a simple report and continue policy, for Symbian OS leaves
 * or exceptions from Qt that fall back to the scheduler.
 * It will be used in cases where there is no existing active scheduler installed.
 * Apps which link to qts60main.lib will have the UI active scheduler installed in the main thread
 * instead of this one. But this would be used in other threads in the UI.
 * An app could replace this behaviour by installing an alternative active scheduler.
 */
void CQtActiveScheduler::Error(TInt aError) const
{
    QT_TRY {
        qWarning("Error from active scheduler %d", aError);
    }
    QT_CATCH (const std::bad_alloc&) {}    // ignore alloc fails, nothing more can be done
}

QT_END_NAMESPACE

#include "moc_qeventdispatcher_symbian_p.cpp"
