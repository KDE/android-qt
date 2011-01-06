/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qelapsedtimer.h"
#include <windows.h>

// Result of QueryPerformanceFrequency, 0 indicates that the high resolution timer is unavailable
static quint64 counterFrequency = 0;

typedef ULONGLONG (WINAPI *PtrGetTickCount64)(void);
static PtrGetTickCount64 ptrGetTickCount64 = 0;

QT_BEGIN_NAMESPACE

static void resolveLibs()
{
    static bool done = false;
    if (done)
        return;

    // try to get GetTickCount64 from the system
    HMODULE kernel32 = GetModuleHandleW(L"kernel32");
    if (!kernel32)
        return;

#if defined(Q_OS_WINCE)
    // does this function exist on WinCE, or will ever exist?
    ptrGetTickCount64 = (PtrGetTickCount64)GetProcAddress(kernel32, L"GetTickCount64");
#else
    ptrGetTickCount64 = (PtrGetTickCount64)GetProcAddress(kernel32, "GetTickCount64");
#endif

    // Retrieve the number of high-resolution performance counter ticks per second
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency)) {
        counterFrequency = 0;
    } else {
        counterFrequency = frequency.QuadPart;
    }

    done = true;
}

static inline qint64 ticksToMilliseconds(qint64 ticks)
{
    if (counterFrequency > 0) {
        // QueryPerformanceCounter uses an arbitrary frequency
        return ticks * 1000 / counterFrequency;
    } else {
        // GetTickCount(64) return milliseconds
        return ticks;
    }
}

static quint64 getTickCount()
{
    resolveLibs();

    // This avoids a division by zero and disables the high performance counter if it's not available
    if (counterFrequency > 0) {
        LARGE_INTEGER counter;

        if (QueryPerformanceCounter(&counter)) {
            return counter.QuadPart;
        } else {
            qWarning("QueryPerformanceCounter failed, although QueryPerformanceFrequency succeeded.");
            return 0;
        }
    }

    if (ptrGetTickCount64)
        return ptrGetTickCount64();

    static quint32 highdword = 0;
    static quint32 lastval = 0;
    quint32 val = GetTickCount();
    if (val < lastval)
        ++highdword;
    lastval = val;
    return val | (quint64(highdword) << 32);
}

QElapsedTimer::ClockType QElapsedTimer::clockType()
{
    resolveLibs();

    if (counterFrequency > 0)
        return PerformanceCounter;
    else
        return TickCounter;
}

bool QElapsedTimer::isMonotonic()
{
    return true;
}

void QElapsedTimer::start()
{
    t1 = getTickCount();
    t2 = 0;
}

qint64 QElapsedTimer::restart()
{
    qint64 oldt1 = t1;
    t1 = getTickCount();
    t2 = 0;
    return ticksToMilliseconds(t1 - oldt1);
}

qint64 QElapsedTimer::elapsed() const
{
    qint64 elapsed = getTickCount() - t1;
    return ticksToMilliseconds(elapsed);
}

qint64 QElapsedTimer::msecsSinceReference() const
{
    return ticksToMilliseconds(t1);
}

qint64 QElapsedTimer::msecsTo(const QElapsedTimer &other) const
{
    qint64 difference = other.t1 - t1;
    return ticksToMilliseconds(difference);
}

qint64 QElapsedTimer::secsTo(const QElapsedTimer &other) const
{
    return msecsTo(other) / 1000;
}

bool operator<(const QElapsedTimer &v1, const QElapsedTimer &v2)
{
    return (v1.t1 - v2.t1) < 0;
}

QT_END_NAMESPACE
