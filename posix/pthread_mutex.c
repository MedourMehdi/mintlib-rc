#include <errno.h>
#include <pthread.h>
#include "pthread_internal.h"
#include <stdio.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    long result;

    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_INIT, (long)mutex, (long)attr);
    return (result < 0) ? -result : 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_LOCK, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    
    /* Use dedicated system call for trylock - don't peek at internal state */
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_TRYLOCK, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_UNLOCK, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    long result;
    
    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_DESTROY, (long)mutex, 0);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr){
    long result;
    
    if (!attr) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_ATTR_INIT, (long)attr, 0);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) 
{
    long result;
    
    if (!attr) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_ATTR_DESTROY, (long)attr, 0);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    long result;
    
    if (!attr) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_SETTYPE, (long)attr, (long)type);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
    long result;
    
    if (!attr) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_SETPROTOCOL, (long)attr, (long)protocol);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) {
    long result;
    
    if (!attr) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_SETPRIOCEILING, (long)attr, (long)prioceiling);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling) {
    long result;

    if (!attr || !prioceiling) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_GETPRIOCEILING, (long)attr, (long)prioceiling);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
    long result;

    if (!attr || !type) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_GETTYPE, (long)attr, (long)type);
    return (result < 0) ? -result : 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol) {
    long result;

    if (!attr || !protocol) 
        return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_MUTEXATTR_GETPROTOCOL, (long)attr, (long)protocol);
    return (result < 0) ? -result : 0;
}