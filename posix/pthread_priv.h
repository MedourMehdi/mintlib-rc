#ifndef _PTHREAD_INTERNAL_H
#define _PTHREAD_INTERNAL_H

#include <pthread.h>
#include <mint/mintbind.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


/* Define the PE_THREAD mode for Pexec */
#define PE_THREAD       107

#define P_PTHREAD 0x185

/* ============================================================================
 * PRIMARY SYSCALL CATEGORIES (sys_p_thread_syscall dispatcher)
 * ============================================================================ */
#define P_THREAD_CTRL       1   /* Thread control operations */
#define P_THREAD_SYNC       2   /* Synchronization operations */
#define P_THREAD_SIGNAL     3   /* Signal operations */

/* Scheduling policy operations (handled directly by dispatcher) */
#define PSCHED_SETPARAM         11   /* Set thread scheduling parameters */
#define PSCHED_GETPARAM         12   /* Get thread scheduling parameters */
#define PSCHED_GETRRINTERVAL    13   /* Get round-robin time slice interval */
#define PSCHED_SET_TIMESLICE    14   /* Set thread time slice */
#define PSCHED_GET_TIMESLICE    15   /* Get thread time slice */

/* Atomic operations (handled directly by dispatcher) */
#define THREAD_ATOMIC_INCREMENT     21   /* Atomic increment */
#define THREAD_ATOMIC_DECREMENT     22   /* Atomic decrement */
#define THREAD_ATOMIC_CAS           23   /* Compare-and-swap */
#define THREAD_ATOMIC_EXCHANGE      24   /* Atomic exchange */
#define THREAD_ATOMIC_ADD           25   /* Atomic addition */
#define THREAD_ATOMIC_SUB           26   /* Atomic subtraction */
#define THREAD_ATOMIC_OR            27   /* Atomic bitwise OR */
#define THREAD_ATOMIC_AND           28   /* Atomic bitwise AND */
#define THREAD_ATOMIC_XOR           29   /* Atomic bitwise XOR */

/* ============================================================================
 * P_THREAD_CTRL OPERATIONS (sys_p_thread_ctrl)
 * ============================================================================ */
#define THREAD_CTRL_EXIT                0   /* Exit the current thread */
#define THREAD_CTRL_CANCEL              1   /* Cancel a thread */
#define THREAD_CTRL_STATUS              4   /* Get thread status */
#define THREAD_CTRL_GETID               5   /* Get thread ID */
#define THREAD_CTRL_SETCANCELSTATE      6   /* Set cancellation state */
#define THREAD_CTRL_SETCANCELTYPE       7   /* Set cancellation type */
#define THREAD_CTRL_TESTCANCEL          8   /* Test for pending cancellation */
#define THREAD_CTRL_SETNAME             9   /* Set thread name */
#define THREAD_CTRL_GETNAME            10   /* Get thread name */
#define THREAD_CTRL_IS_INITIAL         13   /* Check if current thread is initial */
#define THREAD_CTRL_IS_MULTITHREADED   14   /* Check if process is multithreaded */
#define THREAD_CTRL_SWITCH_TO_MAIN     15   /* Switch to main thread context */
#define THREAD_CTRL_SWITCH_TO_THREAD   16   /* Switch to specific thread */

/* ============================================================================
 * P_THREAD_SYNC OPERATIONS (sys_p_thread_sync)
 * ============================================================================
 * Operation Number Ranges:
 * - Semaphore Operations: 1-3
 * - Mutex Operations: 10-14
 * - Mutex Attribute Operations: 20-27
 * - Condition Variable Operations: 30-35
 * - Reader-Writer Lock Operations: 40-46
 * - Thread Lifecycle Operations: 50-52
 * - Thread Scheduling Operations: 60-61
 * - Cleanup Handler Operations: 70-72
 * - Thread-Specific Data Operations: 80-83
 * ============================================================================ */

/* --- Semaphore Operations --- */
#define THREAD_SYNC_SEM_WAIT            1   /* Wait on semaphore (P operation) */
#define THREAD_SYNC_SEM_POST            2   /* Signal semaphore (V operation) */
#define THREAD_SYNC_SEM_INIT            3   /* Initialize semaphore */

/* --- Mutex Operations --- */
#define THREAD_SYNC_MUTEX_INIT          10  /* Initialize mutex */
#define THREAD_SYNC_MUTEX_LOCK          11  /* Lock mutex */
#define THREAD_SYNC_MUTEX_UNLOCK        12  /* Unlock mutex */
#define THREAD_SYNC_MUTEX_TRYLOCK       13  /* Try to lock mutex (non-blocking) */
#define THREAD_SYNC_MUTEX_DESTROY       14  /* Destroy mutex */

/* --- Mutex Attribute Operations --- */
#define THREAD_SYNC_MUTEX_ATTR_INIT        20  /* Initialize mutex attributes */
#define THREAD_SYNC_MUTEX_ATTR_DESTROY     21  /* Destroy mutex attributes */
#define THREAD_SYNC_MUTEXATTR_SETTYPE      22  /* Set mutex type */
#define THREAD_SYNC_MUTEXATTR_GETTYPE      23  /* Get mutex type */
#define THREAD_SYNC_MUTEXATTR_SETPROTOCOL  24  /* Set mutex protocol */
#define THREAD_SYNC_MUTEXATTR_GETPROTOCOL  25  /* Get mutex protocol */
#define THREAD_SYNC_MUTEXATTR_SETPRIOCEILING 26 /* Set priority ceiling */
#define THREAD_SYNC_MUTEXATTR_GETPRIOCEILING 27 /* Get priority ceiling */

/* --- Condition Variable Operations --- */
#define THREAD_SYNC_COND_INIT          30  /* Initialize condition variable */
#define THREAD_SYNC_COND_DESTROY       31  /* Destroy condition variable */
#define THREAD_SYNC_COND_WAIT          32  /* Wait on condition variable */
#define THREAD_SYNC_COND_TIMEDWAIT     33  /* Timed wait on condition variable */
#define THREAD_SYNC_COND_SIGNAL        34  /* Signal condition variable */
#define THREAD_SYNC_COND_BROADCAST     35  /* Broadcast to condition variable */

/* --- Reader-Writer Lock Operations --- */
#define THREAD_SYNC_RWLOCK_INIT        40  /* Initialize reader-writer lock */
#define THREAD_SYNC_RWLOCK_DESTROY     41  /* Destroy reader-writer lock */
#define THREAD_SYNC_RWLOCK_RDLOCK      42  /* Acquire read lock */
#define THREAD_SYNC_RWLOCK_WRLOCK      43  /* Acquire write lock */
#define THREAD_SYNC_RWLOCK_UNLOCK      44  /* Release reader-writer lock */
#define THREAD_SYNC_RWLOCK_TRYRDLOCK   45  /* Try to acquire read lock */
#define THREAD_SYNC_RWLOCK_TRYWRLOCK   46  /* Try to acquire write lock */

/* --- Thread Lifecycle Operations --- */
#define THREAD_SYNC_JOIN               50  /* Join thread and wait for termination */
#define THREAD_SYNC_DETACH             51  /* Detach thread (make unjoinable) */
#define THREAD_SYNC_TRYJOIN            52  /* Non-blocking join attempt */

/* --- Thread Scheduling Operations --- */
#define THREAD_SYNC_SLEEP              60  /* Sleep for specified milliseconds */
#define THREAD_SYNC_YIELD              61  /* Yield CPU to other threads */

/* --- Cleanup Handler Operations --- */
#define THREAD_SYNC_CLEANUP_PUSH       70  /* Push cleanup handler */
#define THREAD_SYNC_CLEANUP_POP        71  /* Pop cleanup handler */
#define THREAD_SYNC_CLEANUP_GET        72  /* Get cleanup handlers */

/* --- Thread-Specific Data Operations --- */
#define THREAD_TSD_CREATE_KEY          80  /* Create TSD key */
#define THREAD_TSD_DELETE_KEY          81  /* Delete TSD key */
#define THREAD_TSD_GET_SPECIFIC        82  /* Get thread-specific data */
#define THREAD_TSD_SET_SPECIFIC        83  /* Set thread-specific data */

/* Thread States (bitfield values) */
#define THREAD_STATE_RUNNING    0x0001  /* Currently executing */
#define THREAD_STATE_READY      0x0002  /* On run queue, can be scheduled */
#define THREAD_STATE_BLOCKED    0x0004  /* Sleeping, waiting for event */
#define THREAD_STATE_STOPPED    0x0008  /* Stopped by signal */
#define THREAD_STATE_ZOMBIE     0x0010  /* Exited, not yet reaped */
#define THREAD_STATE_DEAD       0x0020  /* Fully dead, resources can be freed */

/* Composite states for convenience */
#define THREAD_STATE_EXITED     (THREAD_STATE_ZOMBIE | THREAD_STATE_DEAD)
#define THREAD_STATE_LIVE       (THREAD_STATE_RUNNING | THREAD_STATE_READY)

/* Thread scheduling policies */
#define SCHED_OTHER     0
#define SCHED_FIFO      1
#define SCHED_RR        2

/* ============================================================================
 * P_THREAD_SIGNAL OPERATIONS (sys_p_thread_signal)
 * ============================================================================ */
#define PTSIG_MODE              0   /* Enable/disable thread signals */
#define PTSIG_KILL              1   /* Send signal to specific thread */
#define PTSIG_GETMASK           2   /* Get thread signal mask */
#define PTSIG_SETMASK           3   /* Set thread signal mask */
#define PTSIG_BLOCK             4   /* Block signals */
#define PTSIG_UNBLOCK           5   /* Unblock signals */
#define PTSIG_WAIT              6   /* Wait for signal */
#define PTSIG_HANDLER           7   /* Set signal handler */
#define PTSIG_HANDLER_ARG       8   /* Set signal handler with argument */
#define PTSIG_PENDING           9   /* Get pending signals */
#define PTSIG_ALARM             10   /* Set alarm signal */
#define PTSIG_ALARM_THREAD      11   /* Set alarm signal for specific thread */
#define PTSIG_PAUSE             12   /* Pause with specified mask */
#define PTSIG_BROADCAST         13   /* Broadcast signal to all threads */

/* Magic value for condition variable validation */
#define CONDVAR_MAGIC 0xC0DEC0DE

/* System call wrappers */
/* Direct syscall wrapper - keep this as the base */
static inline long sys_p_pthread(long func, long arg1, long arg2, long arg3) {
    return trap_1_wllll((short)P_PTHREAD, (long)func, (long)arg1, (long)arg2, (long)arg3);
}

/* Flattened syscall wrappers - direct trap calls instead of nested calls */
static inline long sys_p_thread_tsd(long op, long arg1, long arg2){
    return trap_1_wllll((short)P_PTHREAD, (long)P_THREAD_SYNC, (long)op, (long)arg1, (long)arg2);
}

static inline long sys_p_thread_atomic(long op, long arg1, long arg2, long arg3){
    return trap_1_wllll((short)P_PTHREAD, (long)op, (long)arg1, (long)arg2, (long)arg3);
}

static inline long sys_p_thread_sync(long op, long arg1, long arg2) {
    return trap_1_wllll((short)P_PTHREAD, (long)P_THREAD_SYNC, (long)op, (long)arg1, (long)arg2);
}

static inline long proc_thread_signal(long op, long arg1, long arg2) {
    return trap_1_wllll((short)P_PTHREAD, (long)P_THREAD_SIGNAL, (long)op, (long)arg1, (long)arg2);
}

static inline long proc_thread_sleep(long ms) {
    return trap_1_wllll((short)P_PTHREAD, (long)P_THREAD_SYNC, (long)THREAD_SYNC_SLEEP, (long)ms, 0L);
}

static inline long sys_p_thread_ctrl(long mode, long arg1, long arg2) {
    return trap_1_wllll((short)P_PTHREAD, (long)P_THREAD_CTRL, (long)mode, (long)arg1, (long)arg2);
}

static inline long sys_p_thread_sched_policy(long func, long arg1, long arg2, long arg3) {
    return trap_1_wllll((short)P_PTHREAD, (long)func, (long)arg1, (long)arg2, (long)arg3);
}

static inline long proc_thread_status(long arg1) {
    return trap_1_wllll((short)P_PTHREAD, (long)P_THREAD_CTRL, (long)THREAD_CTRL_STATUS, (long)arg1, 0L);
}

/* Internal helper functions */
static inline int validate_cond(pthread_cond_t *cond) {
    pthread_cond_t *internal = (pthread_cond_t *)cond;
    return (internal && internal->magic == CONDVAR_MAGIC && !internal->destroyed);
}

static inline int validate_mutex(pthread_mutex_t *mutex) {
    return (mutex != NULL);
}

static inline int validate_rwlock(pthread_rwlock_t *rwlock) {
    return (rwlock != NULL && *rwlock != 0);
}

#endif /* _PTHREAD_INTERNAL_H */