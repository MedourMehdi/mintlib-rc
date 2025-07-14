#ifndef _PTHREAD_INTERNAL_H
#define _PTHREAD_INTERNAL_H

#include "pthread.h"
#include <mint/mintbind.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* MiNT system call numbers */
#define P_TREAD_SHED        0x185
#define P_THREAD_SYNC       0x18D
#define P_THREAD_CTRL       0x18A
#define P_THREAD_SIGNAL     0x18E
#define P_THREAD_ATOMIC     0x18F

/* Define the PE_THREAD mode for Pexec */
#define PE_THREAD       107

/* Thread operation codes */
#define THREAD_SYNC_SEM_WAIT      1
#define THREAD_SYNC_SEM_POST      2
#define THREAD_SYNC_MUTEX_LOCK    3
#define THREAD_SYNC_MUTEX_UNLOCK  4
#define THREAD_SYNC_MUTEX_INIT    5
#define THREAD_SYNC_SEM_INIT      6
#define THREAD_SYNC_JOIN          7
#define THREAD_SYNC_DETACH        8
#define THREAD_SYNC_TRYJOIN       9
#define THREAD_SYNC_SLEEP         10
#define THREAD_SYNC_YIELD         11
#define THREAD_SYNC_COND_INIT       12
#define THREAD_SYNC_COND_DESTROY    13
#define THREAD_SYNC_COND_WAIT       14
#define THREAD_SYNC_COND_TIMEDWAIT  15
#define THREAD_SYNC_COND_SIGNAL     16
#define THREAD_SYNC_COND_BROADCAST  17
#define THREAD_SYNC_CLEANUP_PUSH    18
#define THREAD_SYNC_CLEANUP_POP     19
#define THREAD_SYNC_CLEANUP_GET     20

/* Thread-specific data operations */
#define THREAD_TSD_CREATE_KEY    21
#define THREAD_TSD_DELETE_KEY    22
#define THREAD_TSD_GET_SPECIFIC  23
#define THREAD_TSD_SET_SPECIFIC  24

/* Reader-writer lock operations */
#define THREAD_SYNC_RWLOCK_INIT      25
#define THREAD_SYNC_RWLOCK_DESTROY   26
#define THREAD_SYNC_RWLOCK_RDLOCK    27
#define THREAD_SYNC_RWLOCK_WRLOCK    28
#define THREAD_SYNC_RWLOCK_UNLOCK    29
#define THREAD_SYNC_RWLOCK_TRYRDLOCK 30
#define THREAD_SYNC_RWLOCK_TRYWRLOCK 31

/* Thread operation modes for sys_p_thread_ctrl */
#define THREAD_CTRL_EXIT     0
#define THREAD_CTRL_CANCEL   1
#define THREAD_CTRL_STATUS   4
#define THREAD_CTRL_GETID    5
#define THREAD_CTRL_SETCANCELSTATE 6
#define THREAD_CTRL_SETCANCELTYPE  7
#define THREAD_CTRL_TESTCANCEL     8
#define THREAD_CTRL_SETNAME  9
#define THREAD_CTRL_GETNAME  10
#define THREAD_CTRL_IS_INITIAL        13
#define THREAD_CTRL_IS_MULTITHREADED  14

/* Atomic operations syscall constants */
#define THREAD_ATOMIC_INCREMENT     1
#define THREAD_ATOMIC_DECREMENT     2
#define THREAD_ATOMIC_CAS           3   /* Compare and swap */
#define THREAD_ATOMIC_EXCHANGE      4   /* Atomic exchange */
#define THREAD_ATOMIC_ADD           5   /* Atomic add */
#define THREAD_ATOMIC_SUB           6   /* Atomic subtraÒct */
#define THREAD_ATOMIC_OR            7   /* Atomic bitwise OR */
#define THREAD_ATOMIC_AND           8   /* Atomic bitwise AND */
#define THREAD_ATOMIC_XOR           9   /* Atomic bitwise XOR */

/* Thread states */
#define THREAD_STATE_RUNNING    0x0001
#define THREAD_STATE_READY      0x0002
#define THREAD_STATE_BLOCKED    0x0004
#define THREAD_STATE_STOPPED    0x0008
#define THREAD_STATE_ZOMBIE     0x0010
#define THREAD_STATE_DEAD       0x0020
#define THREAD_STATE_EXITED     (THREAD_STATE_ZOMBIE | THREAD_STATE_DEAD)
#define THREAD_STATE_LIVE       (THREAD_STATE_RUNNING | THREAD_STATE_READY)

/* Thread scheduling policies */
#define SCHED_OTHER     0
#define SCHED_FIFO      1
#define SCHED_RR        2

/* Thread scheduling operations */
#define PSCHED_SETPARAM       0
#define PSCHED_GETPARAM       1
#define PSCHED_SETSCHEDULER   2
#define PSCHED_GETSCHEDULER   3
#define PSCHED_GETRRINTERVAL  5
#define PSCHED_SET_TIMESLICE  6
#define PSCHED_GET_TIMESLICE  7

/* Signal operations */
typedef enum {
    PTSIG_SETMASK        = 1,
    PTSIG_GETMASK        = 2,
    PTSIG_MODE           = 3,
    PTSIG_KILL           = 4,
    PTSIG_WAIT           = 5,
    PTSIG_BLOCK          = 6,
    PTSIG_UNBLOCK        = 7,
    PTSIG_PAUSE          = 8,
    PTSIG_ALARM          = 9,
    PTSIG_PENDING        = 11,
    PTSIG_HANDLER        = 12,
    PTSIG_HANDLER_ARG    = 14,
    PTSIG_ALARM_THREAD   = 16,
    PTSIG_BROADCAST      = 17
} ptsig_op_t;

/* Magic value for condition variable validation */
#define CONDVAR_MAGIC 0xC0DEC0DE

/* Internal mutex structure */
typedef struct {
    volatile short locked;
    void *owner;
    void *wait_queue;
} pthread_mutex_internal_t;

/* Internal condition variable structure */
typedef struct {
    void *wait_queue;
    void *associated_mutex;
    unsigned long magic;
    int destroyed;
    long timeout_ms;
} pthread_cond_internal_t;

/* Internal thread structure */
struct thread {
    long id;
    void *stack;
};

/* System call wrappers */

static inline long sys_p_thread_tsd(long op, long arg1, long arg2){
    return trap_1_wlll(P_THREAD_SYNC, (long)op, (long)arg1, (long)arg2);
}

static inline long sys_p_thread_atomic(long op, long arg1, long arg2, long arg3){
    return trap_1_wllll(P_THREAD_ATOMIC, (long)op, (long)arg1, (long)arg2, (long)arg3);
}

static inline long sys_p_thread_sync(long op, long arg1, long arg2) {
    return trap_1_wlll(P_THREAD_SYNC, op, arg1, arg2);
}

static inline long proc_thread_signal(int op, long arg1, long arg2) {
    return trap_1_wlll(P_THREAD_SIGNAL, op, arg1, arg2);
}

static inline long proc_thread_sleep(long ms) {
    return sys_p_thread_sync(THREAD_SYNC_SLEEP, ms, 0);
}

static inline long sys_p_thread_ctrl(short mode, long arg1, long arg2) {
    return trap_1_wlll(P_THREAD_CTRL, mode, arg1, arg2);
}

static inline long sys_p_thread_sched_policy(long func, long arg1, long arg2, long arg3) {
    return trap_1_wllll(P_TREAD_SHED, func, arg1, arg2, arg3);
}

static inline long proc_thread_status(long arg1) {
    return sys_p_thread_ctrl(THREAD_CTRL_STATUS, arg1, 0);
}

/* Internal helper functions */
static inline int validate_cond(pthread_cond_t *cond) {
    pthread_cond_internal_t *internal = (pthread_cond_internal_t *)cond;
    return (internal && internal->magic == CONDVAR_MAGIC && !internal->destroyed);
}

static inline int validate_mutex(pthread_mutex_t *mutex) {
    return (mutex != NULL);
}

static inline int validate_rwlock(pthread_rwlock_t *rwlock) {
    return (rwlock != NULL && *rwlock != 0);
}

/* Thread pool structures */
typedef struct thread_pool_task {
    void (*function)(void *);
    void *argument;
    struct thread_pool_task *next;
} thread_pool_task_t;

typedef struct thread_pool {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    thread_pool_task_t *queue;
    int thread_count;
    int queue_size;
    int shutdown;
    int started;
} thread_pool_t;

/* Internal cleanup functions */
void _pthread_cleanup_push_impl(void (*routine)(void*), void *arg);
void _pthread_cleanup_pop_impl(int execute);

/* Internal thread pool functions */
int _thread_pool_destroy(thread_pool_t *pool, int graceful);
int _thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *argument);

/* Thread pool worker function */
void *thread_pool_worker(void *arg);

#endif /* _PTHREAD_INTERNAL_H */