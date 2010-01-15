/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef Q_OS_SYMBIAN
#include <malloc.h>
#endif
#include <limits.h>
#include <stdio.h>
#include <exception>

#if !defined(Q_OS_WIN) && !defined(Q_OS_SYMBIAN)
#  include "3rdparty/memcheck.h"
#endif

static bool mallocFailActive = false;
static int mallocFailIndex = 0;
static int mallocCount = 0;

static void my_terminate_handler()
{
    // set a breakpoint here to get a backtrace for your uncaught exceptions
    fprintf(stderr, "Uncaught Exception Detected. Set a breakpoint in my_terminate_handler()\n");
    exit(1);
}

#ifdef __GLIBC__
/* Use glibc's memory allocation hooks */

/* our hooks */
static void *my_malloc_hook(size_t, const void *);
static void *my_realloc_hook(void *, size_t, const void *);
static void *my_memalign_hook(size_t, size_t, const void *);
static void my_free_hook(void *, const void *);

/* original hooks. */
static void *(*old_malloc_hook)(size_t, const void *);
static void *(*old_realloc_hook)(void *, size_t, const void *);
static void *(*old_memalign_hook)(size_t, size_t, const void *);
static void (*old_free_hook)(void *, const void *);

/* initializer function */
static void my_init_hook();

/* Override initialising hook from the C library. */
void (*__malloc_initialize_hook) (void) = my_init_hook;

static void disableHooks()
{
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __memalign_hook = old_memalign_hook;
    __free_hook = old_free_hook;
}

static void enableHooks()
{
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;
    __memalign_hook = my_memalign_hook;
    __free_hook = my_free_hook;
}

void my_init_hook()
{
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_memalign_hook = __memalign_hook;
    old_free_hook = __free_hook;
    enableHooks();
}

void *my_malloc_hook(size_t size, const void *)
{
    ++mallocCount;

    if (mallocFailActive && --mallocFailIndex < 0)
        return 0; // simulate OOM

    __malloc_hook = old_malloc_hook;
    void *result = ::malloc (size);
    __malloc_hook = my_malloc_hook;

    return result;
}

void *my_memalign_hook(size_t alignment, size_t size, const void *)
{
    ++mallocCount;

    if (mallocFailActive && --mallocFailIndex < 0)
        return 0; // simulate OOM

    __memalign_hook = old_memalign_hook;
    void *result = ::memalign(alignment, size);
    __memalign_hook = my_memalign_hook;

    return result;
}

void *my_realloc_hook(void *ptr, size_t size, const void *)
{
    ++mallocCount;

    if (mallocFailActive && --mallocFailIndex < 0)
        return 0; // simulate OOM

    __realloc_hook = old_realloc_hook;
    __malloc_hook = old_malloc_hook;
    void *result = ::realloc(ptr, size);
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;

    return result;
}

void my_free_hook(void *ptr, const void *)
{
    __free_hook = old_free_hook;
    ::free(ptr);
    __free_hook = my_free_hook;
}

#elif defined(Q_CC_MSVC)

#include "crtdbg.h"

static int qCrtAllocHook(int allocType, void * /*userData*/, size_t /*size*/,
                         int blockType, long /*requestNumber*/,
                         const unsigned char * /*filename*/, int /*lineNumber*/)
{
    if (blockType == _CRT_BLOCK)
        return TRUE; // ignore allocations from the C library

    switch (allocType) {
        case _HOOK_ALLOC:
        case _HOOK_REALLOC:
            ++mallocCount;
            if (mallocFailActive && --mallocFailIndex < 0)
                return FALSE; // simulate OOM
    }

    return TRUE;
}

static struct QCrtDebugRegistrator
{
    QCrtDebugRegistrator()
    {
        _CrtSetAllocHook(qCrtAllocHook);
    }

} crtDebugRegistrator;

#elif defined(Q_OS_SYMBIAN)

struct QAllocFailAllocator : public RAllocator
{
    QAllocFailAllocator() : allocator(User::Allocator())
    {
        User::SwitchAllocator(this);
    }

    ~QAllocFailAllocator()
    {
        User::SwitchAllocator(&allocator);
    }

    RAllocator& allocator;

    // from MAllocator
    TAny* Alloc(TInt aSize)
    {
        ++mallocCount;
        if (mallocFailActive && --mallocFailIndex < 0)
            return 0; // simulate OOM
        return allocator.Alloc(aSize);
    }

    void Free(TAny* aPtr)
    {
        allocator.Free(aPtr);
    }

    TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
    {
        ++mallocCount;
        if (mallocFailActive && --mallocFailIndex < 0)
            return 0; // simulate OOM
        return allocator.ReAlloc(aPtr, aSize, aMode);
    }

    TInt AllocLen(const TAny* aCell) const
    {
        return allocator.AllocLen(aCell);
    }

    TInt Compress()
    {
        return allocator.Compress();
    }

    void Reset()
    {
        allocator.Reset();
    }

    TInt AllocSize(TInt& aTotalAllocSize) const
    {
        return allocator.AllocSize(aTotalAllocSize);
    }

    TInt Available(TInt& aBiggestBlock) const
    {
        return allocator.Available(aBiggestBlock);
    }

    TInt DebugFunction(TInt aFunc, TAny* a1, TAny* a2)
    {
        return allocator.DebugFunction(aFunc, a1, a2);
    }

    TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
    {
        return ((MAllocator&)allocator).Extension_(aExtensionId, a0, a1);
    }
};

QAllocFailAllocator symbianTestAllocator;

#endif

struct AllocFailer
{
    inline AllocFailer(int index) { reactivateAt(index); }
    inline ~AllocFailer() { deactivate(); }

    inline void reactivateAt(int index)
    {
#ifdef RUNNING_ON_VALGRIND
        if (RUNNING_ON_VALGRIND)
            VALGRIND_ENABLE_OOM_AT_ALLOC_INDEX(VALGRIND_GET_ALLOC_INDEX + index + 1);
#endif
        mallocFailIndex = index;
        mallocFailActive = true;
    }

    inline void deactivate()
    {
        mallocFailActive = false;
#ifdef RUNNING_ON_VALGRIND
        VALGRIND_ENABLE_OOM_AT_ALLOC_INDEX(0);
#endif
    }

    inline int currentAllocIndex() const
    {
#ifdef RUNNING_ON_VALGRIND
        if (RUNNING_ON_VALGRIND)
            return VALGRIND_GET_ALLOC_INDEX;
#endif
        return mallocCount;
    }

    static bool initialize()
    {
        std::set_terminate(my_terminate_handler);
#ifdef RUNNING_ON_VALGRIND
        if (RUNNING_ON_VALGRIND) {
            if (VALGRIND_GET_ALLOC_INDEX == -1u) {
                qWarning("You must use a valgrind with oom simulation support");
                return false;
            }
            // running in valgrind - don't use glibc hooks
            disableHooks();

            // never stop simulating OOM
            VALGRIND_DISABLE_OOM_AT_ALLOC_INDEX(-1u);
        }
#endif
         return true;
    }
};

#ifndef Q_OS_SYMBIAN

static void *new_helper(std::size_t size)
{
    void *ptr = malloc(size);
    if (!ptr)
        throw std::bad_alloc();
    return ptr;
}

#ifdef Q_CC_MSVC
#  pragma warning(push)
#  pragma warning(disable: 4290)
#endif

// overload operator new
void* operator new(size_t size) throw (std::bad_alloc) { return new_helper(size); }
void* operator new[](size_t size) throw (std::bad_alloc) { return new_helper(size); }
void* operator new(size_t size, const std::nothrow_t&) throw() { return malloc(size); }
void* operator new[](std::size_t size, const std::nothrow_t&) throw() { return malloc(size); }

// overload operator delete
void operator delete(void *ptr) throw() { if (ptr) free(ptr); }
void operator delete[](void *ptr) throw() { if (ptr) free(ptr); }
void operator delete(void *ptr, const std::nothrow_t&) throw() { if (ptr) free(ptr); }
void operator delete[](void *ptr, const std::nothrow_t&) throw() { if (ptr) free (ptr); }

#ifdef Q_CC_MSVC
#  pragma warning(pop)
#endif

#endif

// ignore placement new and placement delete - those don't allocate.


