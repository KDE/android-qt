#include "qglobal.h"
#include "qbasicatomic.h"

QT_BEGIN_NAMESPACE

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

Q_CORE_EXPORT bool QBasicAtomicInt::isReferenceCountingNative()
{ return true; }
Q_CORE_EXPORT bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return false; }

Q_CORE_EXPORT bool QBasicAtomicInt::isTestAndSetNative()
{ return true; }
Q_CORE_EXPORT bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return false; }

Q_CORE_EXPORT bool QBasicAtomicInt::isFetchAndStoreNative()
{ return true; }
Q_CORE_EXPORT bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return false; }

Q_CORE_EXPORT bool QBasicAtomicInt::isFetchAndAddNative()
{ return true; }
Q_CORE_EXPORT bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return false; }


#ifdef QT_ARCH_ARMV5
// kernel places a restartable cmpxchg implementation at a fixed address
extern "C" typedef int (qt_atomic_eabi_cmpxchg_int_t)(int oldval, int newval, volatile int *ptr);
#define qt_atomic_eabi_cmpxchg_int (*reinterpret_cast<qt_atomic_eabi_cmpxchg_int_t *>(0xffff0fc0))

Q_CORE_EXPORT bool QBasicAtomicInt::ref()
{
    register int originalValue;
    register int newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue + 1;
    } while (qt_atomic_eabi_cmpxchg_int(originalValue, newValue, &_q_value) != 0);
    return newValue != 0;
}

Q_CORE_EXPORT bool QBasicAtomicInt::deref()
{
    register int originalValue;
    register int newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue - 1;
    } while (qt_atomic_eabi_cmpxchg_int(originalValue, newValue, &_q_value) != 0);
    return newValue != 0;
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    register int originalValue;
    do {
        originalValue = _q_value;
        if (originalValue != expectedValue)
            return false;
    } while (qt_atomic_eabi_cmpxchg_int(expectedValue, newValue, &_q_value) != 0);
    return true;
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    register int originalValue;
    do {
      originalValue = _q_value;
    } while (qt_atomic_eabi_cmpxchg_int(originalValue, newValue, &_q_value) != 0);
    return originalValue;
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    register int originalValue;
    register int newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue + valueToAdd;
    } while (qt_atomic_eabi_cmpxchg_int(originalValue, newValue, &_q_value) != 0);
    return originalValue;
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
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

Q_CORE_EXPORT bool QBasicAtomicInt::ref()
{
    register int newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[newValue], [%[_q_value]]\n"
                 "add %[newValue], %[newValue], #1\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

Q_CORE_EXPORT bool QBasicAtomicInt::deref()
{
    register int newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[newValue], [%[_q_value]]\n"
                 "sub %[newValue], %[newValue], #1\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    register int result;
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

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    register int originalValue;
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

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    register int originalValue;
    register int newValue;
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
                 : [valueToAdd] "r" (valueToAdd),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    bool returnValue = testAndSetRelaxed(expectedValue, newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return testAndSetRelaxed(expectedValue, newValue);
}

Q_CORE_EXPORT bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    bool returnValue = testAndSetRelaxed(expectedValue, newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    int returnValue = fetchAndStoreRelaxed(newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndStoreRelaxed(newValue);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    int returnValue = fetchAndStoreRelaxed(newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}


Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    int returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndAddRelaxed(valueToAdd);
}

Q_CORE_EXPORT int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    int returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

#undef Q_DATA_MEMORY_BARRIER
#undef Q_COMPILER_MEMORY_BARRIER

#endif

QT_END_NAMESPACE
