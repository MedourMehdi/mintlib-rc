/**
 * @file osatomic_mint.h
 * @brief OSAtomic-compatible API for FreeMiNT Threading System
 * 
 * Provides OSAtomic-style atomic operations that wrap the FreeMiNT
 * kernel atomic primitives. This header allows code written for
 * macOS/iOS to be more easily ported to FreeMiNT.
 * 
 * Based on Apple's OSAtomic.h but implemented using FreeMiNT syscalls.
 * Only 32-bit operations are supported (no 64-bit on m68k).
 * 
 * @author Medour Mehdi
 * @date January 2026
 * @version 1.0
 */

#ifndef _OSATOMIC_MINT_H_
#define _OSATOMIC_MINT_H_

#include <mint/mintbind.h>
#include <stdint.h>
#include <stdbool.h>

/* Import the pthread syscall definitions */
#ifndef P_PTHREAD
#define P_PTHREAD 0x185
#endif

/* Atomic operation codes (from pthread_priv.h) */
#define THREAD_ATOMIC_INCREMENT     21
#define THREAD_ATOMIC_DECREMENT     22
#define THREAD_ATOMIC_CAS           23
#define THREAD_ATOMIC_EXCHANGE      24
#define THREAD_ATOMIC_ADD           25
#define THREAD_ATOMIC_SUB           26
#define THREAD_ATOMIC_OR            27
#define THREAD_ATOMIC_AND           28
#define THREAD_ATOMIC_XOR           29
#define THREAD_ATOMIC_TAS           30

/* Inline helper for atomic syscalls */
static inline long _mint_atomic(long op, long arg1, long arg2, long arg3) {
    return trap_1_wllll((short)P_PTHREAD, op, arg1, arg2, arg3);
}

/* ============================================================================
 * ARITHMETIC OPERATIONS
 * ============================================================================ */

/**
 * Atomically adds two 32-bit values.
 * @param theAmount The value to add
 * @param theValue Pointer to the value to modify
 * @return The new value after addition
 */
static inline int32_t OSAtomicAdd32(int32_t theAmount, volatile int32_t *theValue) {
    return (int32_t)_mint_atomic(THREAD_ATOMIC_ADD, (long)theValue, (long)theAmount, 0);
}

/**
 * Atomically adds two 32-bit values with memory barrier.
 * Note: FreeMiNT atomic operations already include necessary barriers.
 * @param theAmount The value to add
 * @param theValue Pointer to the value to modify
 * @return The new value after addition
 */
static inline int32_t OSAtomicAdd32Barrier(int32_t theAmount, volatile int32_t *theValue) {
    return OSAtomicAdd32(theAmount, theValue);
}

/**
 * Atomically increments a 32-bit value.
 * @param theValue Pointer to the value to increment
 * @return The new value after increment
 */
static inline int32_t OSAtomicIncrement32(volatile int32_t *theValue) {
    return (int32_t)_mint_atomic(THREAD_ATOMIC_INCREMENT, (long)theValue, 0, 0);
}

/**
 * Atomically increments a 32-bit value with memory barrier.
 * @param theValue Pointer to the value to increment
 * @return The new value after increment
 */
static inline int32_t OSAtomicIncrement32Barrier(volatile int32_t *theValue) {
    return OSAtomicIncrement32(theValue);
}

/**
 * Atomically decrements a 32-bit value.
 * @param theValue Pointer to the value to decrement
 * @return The new value after decrement
 */
static inline int32_t OSAtomicDecrement32(volatile int32_t *theValue) {
    return (int32_t)_mint_atomic(THREAD_ATOMIC_DECREMENT, (long)theValue, 0, 0);
}

/**
 * Atomically decrements a 32-bit value with memory barrier.
 * @param theValue Pointer to the value to decrement
 * @return The new value after decrement
 */
static inline int32_t OSAtomicDecrement32Barrier(volatile int32_t *theValue) {
    return OSAtomicDecrement32(theValue);
}

/* ============================================================================
 * BITWISE OPERATIONS (OR, AND, XOR)
 * ============================================================================ */

/**
 * Atomic bitwise OR of two 32-bit values.
 * @param theMask The mask to OR with
 * @param theValue Pointer to the value to modify
 * @return The new value after OR operation
 */
static inline int32_t OSAtomicOr32(uint32_t theMask, volatile uint32_t *theValue) {
    return (int32_t)_mint_atomic(THREAD_ATOMIC_OR, (long)theValue, (long)theMask, 0);
}

/**
 * Atomic bitwise OR of two 32-bit values with barrier.
 * @param theMask The mask to OR with
 * @param theValue Pointer to the value to modify
 * @return The new value after OR operation
 */
static inline int32_t OSAtomicOr32Barrier(uint32_t theMask, volatile uint32_t *theValue) {
    return OSAtomicOr32(theMask, theValue);
}

/**
 * Atomic bitwise OR returning original value.
 * Note: FreeMiNT atomic_or returns the new value, so we need to calculate original.
 * @param theMask The mask to OR with
 * @param theValue Pointer to the value to modify
 * @return The original value before OR operation
 */
static inline int32_t OSAtomicOr32Orig(uint32_t theMask, volatile uint32_t *theValue) {
    int32_t newval = (int32_t)_mint_atomic(THREAD_ATOMIC_OR, (long)theValue, (long)theMask, 0);
    return newval & ~theMask; /* Remove the bits we just set to get original */
}

/**
 * Atomic bitwise OR returning original value with barrier.
 * @param theMask The mask to OR with
 * @param theValue Pointer to the value to modify
 * @return The original value before OR operation
 */
static inline int32_t OSAtomicOr32OrigBarrier(uint32_t theMask, volatile uint32_t *theValue) {
    return OSAtomicOr32Orig(theMask, theValue);
}

/**
 * Atomic bitwise AND of two 32-bit values.
 * @param theMask The mask to AND with
 * @param theValue Pointer to the value to modify
 * @return The new value after AND operation
 */
static inline int32_t OSAtomicAnd32(uint32_t theMask, volatile uint32_t *theValue) {
    return (int32_t)_mint_atomic(THREAD_ATOMIC_AND, (long)theValue, (long)theMask, 0);
}

/**
 * Atomic bitwise AND of two 32-bit values with barrier.
 * @param theMask The mask to AND with
 * @param theValue Pointer to the value to modify
 * @return The new value after AND operation
 */
static inline int32_t OSAtomicAnd32Barrier(uint32_t theMask, volatile uint32_t *theValue) {
    return OSAtomicAnd32(theMask, theValue);
}

/**
 * Atomic bitwise AND returning original value.
 * Note: FreeMiNT atomic_and returns the new value, so we need to calculate original.
 * @param theMask The mask to AND with
 * @param theValue Pointer to the value to modify
 * @return The original value before AND operation
 */
static inline int32_t OSAtomicAnd32Orig(uint32_t theMask, volatile uint32_t *theValue) {
    int32_t newval = (int32_t)_mint_atomic(THREAD_ATOMIC_AND, (long)theValue, (long)theMask, 0);
    return newval | ~theMask; /* Add back the bits we just cleared to get original */
}

/**
 * Atomic bitwise AND returning original value with barrier.
 * @param theMask The mask to AND with
 * @param theValue Pointer to the value to modify
 * @return The original value before AND operation
 */
static inline int32_t OSAtomicAnd32OrigBarrier(uint32_t theMask, volatile uint32_t *theValue) {
    return OSAtomicAnd32Orig(theMask, theValue);
}

/**
 * Atomic bitwise XOR of two 32-bit values.
 * @param theMask The mask to XOR with
 * @param theValue Pointer to the value to modify
 * @return The new value after XOR operation
 */
static inline int32_t OSAtomicXor32(uint32_t theMask, volatile uint32_t *theValue) {
    return (int32_t)_mint_atomic(THREAD_ATOMIC_XOR, (long)theValue, (long)theMask, 0);
}

/**
 * Atomic bitwise XOR of two 32-bit values with barrier.
 * @param theMask The mask to XOR with
 * @param theValue Pointer to the value to modify
 * @return The new value after XOR operation
 */
static inline int32_t OSAtomicXor32Barrier(uint32_t theMask, volatile uint32_t *theValue) {
    return OSAtomicXor32(theMask, theValue);
}

/**
 * Atomic bitwise XOR returning original value.
 * @param theMask The mask to XOR with
 * @param theValue Pointer to the value to modify
 * @return The original value before XOR operation
 */
static inline int32_t OSAtomicXor32Orig(uint32_t theMask, volatile uint32_t *theValue) {
    int32_t newval = (int32_t)_mint_atomic(THREAD_ATOMIC_XOR, (long)theValue, (long)theMask, 0);
    return newval ^ theMask; /* XOR again to get original */
}

/**
 * Atomic bitwise XOR returning original value with barrier.
 * @param theMask The mask to XOR with
 * @param theValue Pointer to the value to modify
 * @return The original value before XOR operation
 */
static inline int32_t OSAtomicXor32OrigBarrier(uint32_t theMask, volatile uint32_t *theValue) {
    return OSAtomicXor32Orig(theMask, theValue);
}

/* ============================================================================
 * COMPARE AND SWAP OPERATIONS
 * ============================================================================ */

/**
 * Compare and swap for 32-bit values.
 * @param oldValue Expected current value
 * @param newValue Value to set if comparison succeeds
 * @param theValue Pointer to the value to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwap32(int32_t oldValue, int32_t newValue, 
                                             volatile int32_t *theValue) {
    return _mint_atomic(THREAD_ATOMIC_CAS, (long)theValue, (long)oldValue, (long)newValue) != 0;
}

/**
 * Compare and swap for 32-bit values with barrier.
 * @param oldValue Expected current value
 * @param newValue Value to set if comparison succeeds
 * @param theValue Pointer to the value to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwap32Barrier(int32_t oldValue, int32_t newValue, 
                                                     volatile int32_t *theValue) {
    return OSAtomicCompareAndSwap32(oldValue, newValue, theValue);
}

/**
 * Compare and swap for pointers.
 * @param oldValue Expected current pointer value
 * @param newValue Pointer to set if comparison succeeds
 * @param theValue Pointer to the pointer to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwapPtr(void *oldValue, void *newValue, 
                                              void * volatile *theValue) {
    return _mint_atomic(THREAD_ATOMIC_CAS, (long)theValue, (long)oldValue, (long)newValue) != 0;
}

/**
 * Compare and swap for pointers with barrier.
 * @param oldValue Expected current pointer value
 * @param newValue Pointer to set if comparison succeeds
 * @param theValue Pointer to the pointer to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwapPtrBarrier(void *oldValue, void *newValue, 
                                                      void * volatile *theValue) {
    return OSAtomicCompareAndSwapPtr(oldValue, newValue, theValue);
}

/**
 * Compare and swap for int values.
 * @param oldValue Expected current value
 * @param newValue Value to set if comparison succeeds
 * @param theValue Pointer to the value to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwapInt(int oldValue, int newValue, 
                                              volatile int *theValue) {
    return _mint_atomic(THREAD_ATOMIC_CAS, (long)theValue, (long)oldValue, (long)newValue) != 0;
}

/**
 * Compare and swap for int values with barrier.
 * @param oldValue Expected current value
 * @param newValue Value to set if comparison succeeds
 * @param theValue Pointer to the value to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwapIntBarrier(int oldValue, int newValue, 
                                                     volatile int *theValue) {
    return OSAtomicCompareAndSwapInt(oldValue, newValue, theValue);
}

/**
 * Compare and swap for long values.
 * @param oldValue Expected current value
 * @param newValue Value to set if comparison succeeds
 * @param theValue Pointer to the value to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwapLong(long oldValue, long newValue, 
                                               volatile long *theValue) {
    return _mint_atomic(THREAD_ATOMIC_CAS, (long)theValue, oldValue, newValue) != 0;
}

/**
 * Compare and swap for long values with barrier.
 * @param oldValue Expected current value
 * @param newValue Value to set if comparison succeeds
 * @param theValue Pointer to the value to modify
 * @return true if swap occurred, false otherwise
 */
static inline bool OSAtomicCompareAndSwapLongBarrier(long oldValue, long newValue, 
                                                      volatile long *theValue) {
    return OSAtomicCompareAndSwapLong(oldValue, newValue, theValue);
}

/* ============================================================================
 * TEST AND SET/CLEAR OPERATIONS
 * ============================================================================ */

/**
 * Atomic test and set.
 * Tests a bit and sets it if not already set.
 * 
 * @param n Bit number (0-7 within a byte, higher bits select byte offset)
 * @param theAddress Base address of memory location
 * @return The original value of the bit being tested (0 or non-zero)
 */
static inline bool OSAtomicTestAndSet(uint32_t n, volatile void *theAddress) {
    /* Calculate byte address and bit mask */
    volatile unsigned short *wordAddr = (volatile unsigned short *)
                                         ((char *)theAddress + (n >> 3));
    
    /* Use TAS on the appropriate word-aligned address */
    return _mint_atomic(THREAD_ATOMIC_TAS, (long)wordAddr, 0, 0) != 0;
}

/**
 * Atomic test and set with barrier.
 * @param n Bit number
 * @param theAddress Base address of memory location
 * @return The original value of the bit being tested
 */
static inline bool OSAtomicTestAndSetBarrier(uint32_t n, volatile void *theAddress) {
    return OSAtomicTestAndSet(n, theAddress);
}

/**
 * Atomic test and clear.
 * Tests a bit and clears it if set.
 * 
 * Note: This is implemented using AND operation since we don't have
 * a dedicated TAS-clear instruction.
 * 
 * @param n Bit number (0-7 within a byte, higher bits select byte offset)
 * @param theAddress Base address of memory location
 * @return The original value of the bit being tested
 */
static inline bool OSAtomicTestAndClear(uint32_t n, volatile void *theAddress) {
    volatile uint32_t *addr = (volatile uint32_t *)theAddress;
    uint32_t bit = (0x80000000U >> (n & 31));
    int32_t oldval = OSAtomicAnd32Orig(~bit, addr);
    return (oldval & bit) != 0;
}

/**
 * Atomic test and clear with barrier.
 * @param n Bit number
 * @param theAddress Base address of memory location
 * @return The original value of the bit being tested
 */
static inline bool OSAtomicTestAndClearBarrier(uint32_t n, volatile void *theAddress) {
    return OSAtomicTestAndClear(n, theAddress);
}

/* ============================================================================
 * MEMORY BARRIER
 * ============================================================================ */

/**
 * Memory barrier.
 * On m68k, this is typically a no-op for single-processor systems,
 * but we provide it for API compatibility.
 */
static inline void OSMemoryBarrier(void) {
    /* Memory barrier - compiler fence */
    __asm__ volatile ("" : : : "memory");
}

/* ============================================================================
 * CONVENIENCE MACROS
 * ============================================================================ */

/* For code that checks if atomics are available */
#define OS_ATOMIC_AVAILABLE 1

/* Note: 64-bit operations are NOT supported on m68k */
#define OS_ATOMIC_64BIT_AVAILABLE 0

#endif /* _OSATOMIC_MINT_H_ */