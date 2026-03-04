#ifndef _PTHREAD_H
#define _PTHREAD_H

/* Include cdefs for __THROW definition. 
 * This is standard in GCC environments. */
#include <sys/cdefs.h>

/* Fallback definition if sys/cdefs.h is missing or incomplete */
#ifndef __THROW
# if defined __cplusplus
#  define __THROW throw ()
# else
#  define __THROW
# endif
#endif

#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Thread ID type */
typedef long pthread_t;

/* Thread cancellation constants */
#define PTHREAD_CANCEL_ENABLE       0
#define PTHREAD_CANCEL_DISABLE      1
#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_CANCELED           ((void *)-1)

#define PTHREAD_BARRIER_SERIAL_THREAD 1

/* Thread attribute type */
typedef struct {
    size_t stacksize;
    int detachstate;
    int policy;
    int priority;
    int inheritsched;
} pthread_attr_t;

/* Mutex types */
#define PTHREAD_MUTEX_NORMAL      0
#define PTHREAD_MUTEX_RECURSIVE   1
#define PTHREAD_MUTEX_ERRORCHECK  2

/* Priority protocols */
#define PTHREAD_PRIO_NONE         0
#define PTHREAD_PRIO_INHERIT      1
#define PTHREAD_PRIO_PROTECT      2

/* Mutex types */
typedef struct {
    void *owner;
    void *wait_queue;    
    short locked;
    short protocol;              /* Priority protocol */
    short type;                  /* Mutex type */
    short prioceiling;           /* Priority ceiling */
    short saved_priority;        /* Saved priority for ceiling protocol */
    short lock_count;            /* Lock count for recursive mutexes */
} pthread_mutex_t;

typedef struct {
    short type;                 /* Mutex type: NORMAL, RECURSIVE, ERRORCHECK */
    short pshared;              /* Process-shared flag */
    short protocol;             /* Priority protocol */
    short prioceiling;          /* Priority ceiling value */
} pthread_mutexattr_t;

/* Condition variable */
typedef struct {
    void *wait_queue;      /* Queue of threads waiting on this condvar */
    struct mutex *associated_mutex; /* Mutex associated with this condvar */
    unsigned long magic;            /* Magic number for validation */
    int destroyed;                  /* Flag indicating if condvar is destroyed */   
    long timeout_ms;                /* Timeout value in milliseconds */
} pthread_cond_t;

typedef struct {
    int type;
} pthread_condattr_t;

/* Read-write lock */
typedef long pthread_rwlock_t;
typedef struct {
    int type;
} pthread_rwlockattr_t;

/* Thread-specific data */
typedef unsigned int pthread_key_t;
typedef long pthread_once_t;

/* Barrier */
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    unsigned int count;
    unsigned int waiting;
    unsigned int generation;
} pthread_barrier_t;

typedef struct {
    int pshared;
} pthread_barrierattr_t;

/* Spinlock */
typedef int pthread_spinlock_t;

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

/* Constants & Initializers */
#define PTHREAD_INHERIT_SCHED   0
#define PTHREAD_EXPLICIT_SCHED  1

#define PTHREAD_CREATE_JOINABLE  0
#define PTHREAD_CREATE_DETACHED  1

#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

#define PTHREAD_ONCE_INIT        0L
#define PTHREAD_MUTEX_INITIALIZER {NULL, NULL, 0, 0, 0, 0, 0, 0}
#define PTHREAD_RWLOCK_INITIALIZER 0
#define PTHREAD_COND_INITIALIZER {NULL, NULL, 0xC0DEC0DE, 0, 0}

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1

/* In many libs, PTHREAD_STACK_MIN is 16KB, but hardcoding is okay for now */
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384
#endif

/* ==================== */
/* Thread Management API */
/* ==================== */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                  void *(*start_routine)(void*), void *arg) __THROW;
void pthread_exit(void *retval) __THROW;
int pthread_join(pthread_t thread, void **retval) __THROW;
int pthread_tryjoin_np(pthread_t thread, void **retval) __THROW;
int pthread_detach(pthread_t thread) __THROW;
pthread_t pthread_self(void) __THROW;
int pthread_equal(pthread_t t1, pthread_t t2) __THROW;
int pthread_yield(void) __THROW;

/* Thread Attributes */
int pthread_attr_init(pthread_attr_t *attr) __THROW;
int pthread_attr_destroy(pthread_attr_t *attr) __THROW;
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) __THROW;
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) __THROW;
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) __THROW;
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) __THROW;
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) __THROW;
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) __THROW;
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) __THROW;
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) __THROW;
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) __THROW;
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) __THROW;

/* ====================== */
/* Mutex Management API */
/* ====================== */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) __THROW;
int pthread_mutex_destroy(pthread_mutex_t *mutex) __THROW;
int pthread_mutex_lock(pthread_mutex_t *mutex) __THROW;
int pthread_mutex_trylock(pthread_mutex_t *mutex) __THROW;
int pthread_mutex_unlock(pthread_mutex_t *mutex) __THROW;

/* Mutex Attributes */
int pthread_mutexattr_init(pthread_mutexattr_t *attr) __THROW;
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) __THROW;
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) __THROW;
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) __THROW;
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) __THROW;
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol) __THROW;
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) __THROW;
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling) __THROW;

/* ====================== */
/* Condition Variables API */
/* ====================== */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) __THROW;
int pthread_cond_destroy(pthread_cond_t *cond) __THROW;
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) __THROW;
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                          const struct timespec *abstime) __THROW;
int pthread_cond_signal(pthread_cond_t *cond) __THROW;
int pthread_cond_broadcast(pthread_cond_t *cond) __THROW;

/* Condition Attributes */
int pthread_condattr_init(pthread_condattr_t *attr) __THROW;
int pthread_condattr_destroy(pthread_condattr_t *attr) __THROW;

/* ====================== */
/* Read-Write Locks API */
/* ====================== */
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) __THROW;
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) __THROW;
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) __THROW;
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) __THROW;
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) __THROW;
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) __THROW;
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) __THROW;

/* ====================== */
/* Barrier API */
/* ====================== */
int pthread_barrier_init(pthread_barrier_t *barrier, 
                        const pthread_barrierattr_t *attr,
                        unsigned int count) __THROW;
int pthread_barrier_destroy(pthread_barrier_t *barrier) __THROW;
int pthread_barrier_wait(pthread_barrier_t *barrier) __THROW;

/* ====================== */
/* Spinlock API */
/* ====================== */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) __THROW;
int pthread_spin_destroy(pthread_spinlock_t *lock) __THROW;
int pthread_spin_lock(pthread_spinlock_t *lock) __THROW;
int pthread_spin_trylock(pthread_spinlock_t *lock) __THROW;
int pthread_spin_unlock(pthread_spinlock_t *lock) __THROW;
int pthread_spin_attach(pthread_spinlock_t *lock, const char *shm_path) __THROW;

/* ====================== */
/* Thread-Specific Data API */
/* ====================== */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) __THROW;
int pthread_key_delete(pthread_key_t key) __THROW;
void *pthread_getspecific(pthread_key_t key) __THROW;
int pthread_setspecific(pthread_key_t key, const void *value) __THROW;
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) __THROW;

/* ====================== */
/* Cancellation API */
/* ====================== */
int pthread_cancel(pthread_t thread) __THROW;
int pthread_setcancelstate(int state, int *oldstate) __THROW;
int pthread_setcanceltype(int type, int *oldtype) __THROW;
void pthread_testcancel(void) __THROW;

/* ====================== */
/* Scheduling API */
/* ====================== */
int pthread_setschedparam(pthread_t thread, int policy, 
                         const struct sched_param *param) __THROW;
int pthread_getschedparam(pthread_t thread, int *policy,
                         struct sched_param *param) __THROW;
int pthread_setschedprio(pthread_t thread, int prio) __THROW;
int pthread_getconcurrency(void) __THROW;
int pthread_setconcurrency(int level) __THROW;

/* ====================== */
/* Signal Handling API */
/* ====================== */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset) __THROW;
int pthread_kill(pthread_t thread, int sig) __THROW;
int pthread_sigwait(const sigset_t *set, int *sig) __THROW;
int pthread_sigtimedwait(const sigset_t *set, int *sig, long timeout) __THROW;
int pthread_kill_all(int sig) __THROW;

int pthread_sigqueue(pthread_t thread, int sig, const union sigval value) __THROW;
int pthread_sigpending(sigset_t *set) __THROW;
int pthread_sigpause(unsigned long mask) __THROW;

/* ====================== */
/* Non-Portable Extensions */
/* ====================== */
int pthread_setname_np(pthread_t thread, const char *name) __THROW;
int pthread_getname_np(pthread_t thread, char *name, size_t len) __THROW;
int pthread_is_initialthread_np(void) __THROW;
int pthread_is_multithreaded_np(void) __THROW;
int pthread_setup_threading_np(void) __THROW;

/* ====================== */
/* Utility Functions */
/* ====================== */
int msleep(long ms) __THROW;
int __msleep(long ms) __THROW;

/* Cleanup Handlers */
/* Note: In POSIX, these are macros, but implemented as functions here */
void pthread_cleanup_push(void (*routine)(void*), void *arg) __THROW;
void pthread_cleanup_pop(int execute) __THROW;

/* Thread Pool API (Non-standard) */
typedef struct thread_pool thread_pool_t;
thread_pool_t *thread_pool_create(int thread_count) __THROW;
int thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *argument) __THROW;
int thread_pool_destroy(thread_pool_t *pool, int graceful) __THROW;

extern short __mint_is_multithreaded;

#ifdef __cplusplus
}
#endif

#endif /* _PTHREAD_H */