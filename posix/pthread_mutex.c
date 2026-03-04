#include <errno.h>
#include <pthread.h>
#include "pthread_priv.h"
#include <stdio.h>

/* =========================== */
/*    pthread_mutex_init       */
/* =========================== */

__typeof__(pthread_mutex_init) __pthread_mutex_init;

int __pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    long result;

    if (!mutex) return EINVAL;
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_INIT, (long)mutex, (long)attr);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutex_init, pthread_mutex_init)


/* =========================== */
/*    pthread_mutex_lock       */
/* =========================== */

__typeof__(pthread_mutex_lock) __pthread_mutex_lock;

int __pthread_mutex_lock(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_LOCK, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutex_lock, pthread_mutex_lock)


/* =========================== */
/*  pthread_mutex_trylock      */
/* =========================== */

__typeof__(pthread_mutex_trylock) __pthread_mutex_trylock;

int __pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    /* Use dedicated system call for trylock - don't peek at internal state */
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_TRYLOCK, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutex_trylock, pthread_mutex_trylock)


/* =========================== */
/*   pthread_mutex_unlock      */
/* =========================== */

__typeof__(pthread_mutex_unlock) __pthread_mutex_unlock;

int __pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_UNLOCK, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutex_unlock, pthread_mutex_unlock)


/* =========================== */
/*  pthread_mutex_destroy      */
/* =========================== */

__typeof__(pthread_mutex_destroy) __pthread_mutex_destroy;

int __pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_DESTROY, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutex_destroy, pthread_mutex_destroy)


/* =========================== */
/*  pthread_mutexattr_init     */
/* =========================== */

__typeof__(pthread_mutexattr_init) __pthread_mutexattr_init;

int __pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    long result;
    
    if (!attr) return EINVAL;
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_ATTR_INIT, (long)attr, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_init, pthread_mutexattr_init)


/* =========================== */
/* pthread_mutexattr_destroy   */
/* =========================== */

__typeof__(pthread_mutexattr_destroy) __pthread_mutexattr_destroy;

int __pthread_mutexattr_destroy(pthread_mutexattr_t *attr) 
{
    long result;
    
    if (!attr) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_ATTR_DESTROY, (long)attr, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_destroy, pthread_mutexattr_destroy)


/* =========================== */
/* pthread_mutexattr_settype   */
/* =========================== */

__typeof__(pthread_mutexattr_settype) __pthread_mutexattr_settype;

int __pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) 
{
    long result;
    
    if (!attr) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_SETTYPE, (long)attr, (long)type);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_settype, pthread_mutexattr_settype)


/* =========================== */
/* pthread_mutexattr_setprotocol */
/* =========================== */

__typeof__(pthread_mutexattr_setprotocol) __pthread_mutexattr_setprotocol;

int __pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) 
{
    long result;
    
    if (!attr) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_SETPROTOCOL, (long)attr, (long)protocol);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_setprotocol, pthread_mutexattr_setprotocol)


/* =========================== */
/* pthread_mutexattr_setprioceiling */
/* =========================== */

__typeof__(pthread_mutexattr_setprioceiling) __pthread_mutexattr_setprioceiling;

int __pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) 
{
    long result;
    
    if (!attr) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_SETPRIOCEILING, (long)attr, (long)prioceiling);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_setprioceiling, pthread_mutexattr_setprioceiling)


/* =========================== */
/* pthread_mutexattr_getprioceiling */
/* =========================== */

__typeof__(pthread_mutexattr_getprioceiling) __pthread_mutexattr_getprioceiling;

int __pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling) 
{
    long result;

    if (!attr || !prioceiling) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_GETPRIOCEILING, (long)attr, (long)prioceiling);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_getprioceiling, pthread_mutexattr_getprioceiling)


/* =========================== */
/* pthread_mutexattr_gettype   */
/* =========================== */

__typeof__(pthread_mutexattr_gettype) __pthread_mutexattr_gettype;

int __pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) 
{
    long result;

    if (!attr || !type) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_GETTYPE, (long)attr, (long)type);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_gettype, pthread_mutexattr_gettype)


/* =========================== */
/* pthread_mutexattr_getprotocol */
/* =========================== */

__typeof__(pthread_mutexattr_getprotocol) __pthread_mutexattr_getprotocol;

int __pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol) 
{
    long result;

    if (!attr || !protocol) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_GETPROTOCOL, (long)attr, (long)protocol);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_mutexattr_getprotocol, pthread_mutexattr_getprotocol)