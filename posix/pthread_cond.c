#include <errno.h>
#include <pthread.h>
#include "pthread_priv.h"

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    long result;

    if (!cond) return EINVAL;
    cond->wait_queue = NULL;
    cond->associated_mutex = NULL;
    cond->magic = CONDVAR_MAGIC;
    cond->destroyed = 0;
    cond->timeout_ms = 0;
    
    result = sys_p_thread_sync(THREAD_SYNC_COND_INIT, (long)cond, 0);
    if (result < 0) return -result;
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    long result;

    if (!cond || cond->magic != CONDVAR_MAGIC) return EINVAL;
    if (cond->wait_queue) return EBUSY;
    
    result = sys_p_thread_sync(THREAD_SYNC_COND_DESTROY, (long)cond, 0);
    cond->magic = 0;
    cond->destroyed = 1;
    return (result < 0) ? -result : 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    long result;
        
    if (!cond || !mutex || cond->magic != CONDVAR_MAGIC) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_COND_WAIT, (long)cond, (long)mutex);
    return (result < 0) ? -result : 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    long result;
    
    if (!cond || cond->magic != CONDVAR_MAGIC) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_COND_SIGNAL, (long)cond, 0);
    return (result < 0) ? -result : 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    long result;
    
    if (!cond || cond->magic != CONDVAR_MAGIC) return EINVAL;
    result = sys_p_thread_sync(THREAD_SYNC_COND_BROADCAST, (long)cond, 0);
    return (result < 0) ? -result : 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, 
                          const struct timespec *abstime)
{
    long result, ms;
    
    if (!cond || !mutex || cond->magic != CONDVAR_MAGIC || !abstime)
        return EINVAL;
    
    ms = abstime->tv_sec * 1000 + abstime->tv_nsec / 1000000;
    if (ms <= 0) return ETIMEDOUT;
    
    cond->timeout_ms = ms;
    result = sys_p_thread_sync(THREAD_SYNC_COND_TIMEDWAIT, (long)cond, (long)mutex);
    return (result < 0) ? -result : 0;
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    if (!attr) return EINVAL;
    attr->type = 0;
    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    if (!attr) return EINVAL;
    return 0;
}