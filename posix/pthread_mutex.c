#include <errno.h>
#include <pthread.h>
#include "pthread_internal.h"

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    long result;

    if (!mutex) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_MUTEX_INIT, (long)mutex, 0);
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
    if (!mutex) return EINVAL;
    if (mutex->locked) return EBUSY;
    return pthread_mutex_lock(mutex);
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
    if (!mutex) return EINVAL;
    if (mutex->locked) return EBUSY;
    mutex->locked = 0;
    mutex->owner = NULL;
    mutex->wait_queue = NULL;
    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (!attr) return EINVAL;
    attr->type = PTHREAD_MUTEX_DEFAULT;
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if (!attr) return EINVAL;
    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if (!attr || (type != PTHREAD_MUTEX_NORMAL && 
                 type != PTHREAD_MUTEX_RECURSIVE && 
                 type != PTHREAD_MUTEX_ERRORCHECK))
        return EINVAL;
    attr->type = type;
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    if (!attr || !type) return EINVAL;
    *type = attr->type;
    return 0;
}