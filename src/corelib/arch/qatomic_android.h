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

#ifndef QATOMIC_ANDROID_H
#define QATOMIC_ANDROID_H

QT_BEGIN_HEADER

#if defined(__ARM_ARCH_7__) \
    || defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7R__) \
    || defined(__ARM_ARCH_7M__)
# define QT_ARCH_ARMV7
#elif defined(__ARM_ARCH_6__) \
    || defined(__ARM_ARCH_6J__) \
    || defined(__ARM_ARCH_6T2__) \
    || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6K__) \
    || defined(__ARM_ARCH_6ZK__) \
    || defined(__ARM_ARCH_6M__) \
    || (defined(__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM-0 >= 6))
# define QT_ARCH_ARMV6
#else
# define QT_ARCH_ARMV5
#endif

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_WAIT_FREE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return true; }
template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return true; }
template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return true; }
template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return true; }

#ifdef QT_ARCH_ARMV5

extern "C" typedef int (qt_atomic_eabi_cmpxchg_ptr_t)(const void *oldval, const void *newval, volatile void *ptr);
#define qt_atomic_eabi_cmpxchg_ptr (*reinterpret_cast<qt_atomic_eabi_cmpxchg_ptr_t *>(0xffff0fc0))

// Test and set for pointers

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
#ifdef QT_USE_FAST_ATOMICS
    register T *originalValue;
    do {
        originalValue = _q_value;
        if (originalValue != expectedValue)
            return false;
    } while (qt_atomic_eabi_cmpxchg_ptr(expectedValue, newValue, &_q_value) != 0);
    return true;
#else
    return __sync_bool_compare_and_swap(&_q_value, expectedValue, newValue);
#endif
}

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

// Fetch and store for pointers

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
#ifdef QT_USE_FAST_ATOMICS
    T *originalValue;
    asm volatile(
#if defined(__thumb__) && !defined(thumb2)
                 "adr r4,1f\n"
                 "bx r3\n"
                 ".align\n"
                 ".arm\n"
                 "1:\n"
#endif
                 "swp %0,%2,[%3]\n"
#if defined(__thumb__) && !defined(thumb2)
                 "adr r3, 2f+1\n"
                 "bx r3\n"
                 ".thumb\n"
                 "2:\n"
#endif
                 : "=&r"(originalValue), "=m" (_q_value)
                 : "r"(newValue), "r"(&_q_value)
                 : "cc", "memory");
    return originalValue;
#else
    return __sync_lock_test_and_set(&_q_value, newValue);
#endif
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

// Fetch and add for pointers

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
#ifdef QT_USE_FAST_ATOMICS
    register T *originalValue;
    register T *newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue + valueToAdd;
    } while (qt_atomic_eabi_cmpxchg_ptr(originalValue, newValue, &_q_value) != 0);
    return originalValue;
#else
    return __sync_fetch_and_add(&_q_value, valueToAdd * sizeof(T));
#endif
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

#else

#ifdef QT_ARCH_ARMV6
# define Q_DATA_MEMORY_BARRIER asm volatile("":::"memory")
#else
# ifndef Q_NO_SMP_SAFE_ATOMICS
#  define Q_DATA_MEMORY_BARRIER asm volatile("dmb\n":::"memory")
# else
#   define Q_DATA_MEMORY_BARRIER asm volatile("":::"memory")
# endif
#endif

#define Q_COMPILER_MEMORY_BARRIER asm volatile("":::"memory")

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    register T *result;
    asm volatile("0:\n"
                 "ldrex %[result], [%[_q_value]]\n"
                 "eors %[result], %[result], %[expectedValue]\n"
                 "itt eq\n"
                 "strexeq %[result], %[newValue], [%[_q_value]]\n"
                 "teqeq %[result], #1\n"
                 "beq 0b\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return result == 0;
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    register T *originalValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[originalValue], [%[_q_value]]\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    register T *originalValue;
    register T *newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[originalValue], [%[_q_value]]\n"
                 "add %[newValue], %[originalValue], %[valueToAdd]\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [valueToAdd] "r" (valueToAdd * sizeof(T)),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    bool returnValue = testAndSetRelaxed(expectedValue, newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return testAndSetRelaxed(expectedValue, newValue);
}

template <typename T> Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    bool returnValue = testAndSetAcquire(expectedValue, newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    T *returnValue = fetchAndStoreRelaxed(newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndStoreRelaxed(newValue);
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    T *returnValue = fetchAndStoreRelaxed(newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    T *returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndAddRelaxed(valueToAdd);
}

template <typename T> Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    T *returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}
#undef Q_DATA_MEMORY_BARRIER
#undef Q_COMPILER_MEMORY_BARRIER

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_ANDROID_H
