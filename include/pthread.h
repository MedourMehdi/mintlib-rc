#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <errno.h>

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
    int detachstate;
    size_t stacksize;
    int policy;
    int priority;
} pthread_attr_t;

/* Mutex types */
typedef struct {
    volatile short locked;
    void* owner;
    void* wait_queue;
} pthread_mutex_t;

typedef struct {
    int type;
} pthread_mutexattr_t;

/* Condition variable */
typedef struct {
    void* wait_queue;
    void* associated_mutex;
    unsigned long magic;
    int destroyed;
    long timeout_ms;
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
typedef int pthread_once_t;

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

/* Scheduling */
// struct sched_param {
//     int sched_priority;
// };

/* Constants & Initializers */
#define PTHREAD_CREATE_JOINABLE  0
#define PTHREAD_CREATE_DETACHED  1

#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

#define PTHREAD_ONCE_INIT        0
#define PTHREAD_MUTEX_INITIALIZER {0, 0, NULL}
#define PTHREAD_RWLOCK_INITIALIZER 0
#define PTHREAD_COND_INITIALIZER {NULL, NULL, 0xC0DEC0DE, 0, 0}

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1

/* ==================== */
/* Thread Management API */
/* ==================== */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                  void *(*start_routine)(void*), void *arg);
void pthread_exit(void *retval);
int pthread_join(pthread_t thread, void **retval);
int pthread_tryjoin_np(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);
pthread_t pthread_self(void);
int pthread_equal(pthread_t t1, pthread_t t2);
int pthread_yield(void);

/* Thread Attributes */
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);

/* ====================== */
/* Mutex Management API */
/* ====================== */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/* Mutex Attributes */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);

/* ====================== */
/* Condition Variables API */
/* ====================== */
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                          const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

/* Condition Attributes */
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);

/* ====================== */
/* Read-Write Locks API */
/* ====================== */
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

/* ====================== */
/* Barrier API */
/* ====================== */
int pthread_barrier_init(pthread_barrier_t *barrier, 
                        const pthread_barrierattr_t *attr,
                        unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);

/* ====================== */
/* Spinlock API */
/* ====================== */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
// int pthread_spin_attach(pthread_spinlock_t *lock);
int pthread_spin_attach(pthread_spinlock_t *lock, const char *shm_path);

/* ====================== */
/* Thread-Specific Data API */
/* ====================== */
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/* ====================== */
/* Cancellation API */
/* ====================== */
int pthread_cancel(pthread_t thread);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel(void);

/* ====================== */
/* Scheduling API */
/* ====================== */
int pthread_setschedparam(pthread_t thread, int policy, 
                         const struct sched_param *param);
int pthread_getschedparam(pthread_t thread, int *policy,
                         struct sched_param *param);
int pthread_setschedprio(pthread_t thread, int prio);
int pthread_getconcurrency(void);
int pthread_setconcurrency(int level);

/* ====================== */
/* Signal Handling API */
/* ====================== */
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
int pthread_kill(pthread_t thread, int sig);
int pthread_sigwait(const sigset_t *set, int *sig);
int pthread_sigtimedwait(const sigset_t *set, int *sig, long timeout);
int pthread_kill_all(int sig);

/* ====================== */
/* Non-Portable Extensions */
/* ====================== */
int pthread_setname_np(pthread_t thread, const char *name);
int pthread_getname_np(pthread_t thread, char *name, size_t len);
int pthread_is_initialthread_np(void);
int pthread_is_multithreaded_np(void);

/* ====================== */
/* Utility Functions */
/* ====================== */
int msleep(long ms);

/* Cleanup Handlers */
void pthread_cleanup_push(void (*routine)(void*), void *arg);
void pthread_cleanup_pop(int execute);

/* Thread Pool API (Non-standard) */
typedef struct thread_pool thread_pool_t;
thread_pool_t *thread_pool_create(int thread_count);
int thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *argument);
int thread_pool_destroy(thread_pool_t *pool, int graceful);

// /* Atomic Operations (Non-standard) */
// int atomic_increment(volatile int *value);
// int atomic_decrement(volatile int *value);
// int atomic_cas(volatile int *ptr, int oldval, int newval);

#ifdef __cplusplus
}
#endif

#endif /* _PTHREAD_H */