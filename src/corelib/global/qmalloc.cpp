/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qplatformdefs.h"

#include <stdlib.h>

/*
    Define the container allocation functions in a separate file, so that our
    users can easily override them.
*/

QT_BEGIN_NAMESPACE

void *qMalloc(size_t size)
{
    return ::malloc(size);
}

void qFree(void *ptr)
{
    ::free(ptr);
}

void *qRealloc(void *ptr, size_t size)
{
    return ::realloc(ptr, size);
}

void *qMallocAligned(size_t size, size_t alignment)
{
    return qReallocAligned(0, size, 0, alignment);
}

void *qReallocAligned(void *oldptr, size_t newsize, size_t oldsize, size_t alignment)
{
    // fake an aligned allocation
    Q_UNUSED(oldsize);

    void *actualptr = oldptr ? static_cast<void **>(oldptr)[-1] : 0;
    if (alignment <= sizeof(void*)) {
        // special, fast case
        void **newptr = static_cast<void **>(qRealloc(actualptr, newsize + sizeof(void*)));
        if (!newptr)
            return 0;
        if (newptr == actualptr) {
            // realloc succeeded without reallocating
            return oldptr;
        }

        *newptr = newptr;
        return newptr + 1;
    }

    // qMalloc returns pointers aligned at least at sizeof(size_t) boundaries
    // but usually more (8- or 16-byte boundaries).
    // So we overallocate by alignment-sizeof(size_t) bytes, so we're guaranteed to find a
    // somewhere within the first alignment-sizeof(size_t) that is properly aligned.

    // However, we need to store the actual pointer, so we need to allocate actually size +
    // alignment anyway.

    void *real = qRealloc(actualptr, newsize + alignment);
    if (!real)
        return 0;

    quintptr faked = reinterpret_cast<quintptr>(real) + alignment;
    faked &= ~(alignment - 1);

    void **faked_ptr = reinterpret_cast<void **>(faked);

    // now save the value of the real pointer at faked-sizeof(void*)
    // by construction, alignment > sizeof(void*) and is a power of 2, so
    // faked-sizeof(void*) is properly aligned for a pointer
    faked_ptr[-1] = real;

    return faked_ptr;
}

void qFreeAligned(void *ptr)
{
    if (!ptr)
        return;
    void **ptr2 = static_cast<void **>(ptr);
    free(ptr2[-1]);
}

QT_END_NAMESPACE

