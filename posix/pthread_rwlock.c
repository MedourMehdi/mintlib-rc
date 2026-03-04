#include <pthread.h>
#include "pthread_priv.h"
#include <errno.h>

/* =========================== */
/*    pthread_rwlock_init      */
/* =========================== */

__typeof__(pthread_rwlock_init) __pthread_rwlock_init;

int __pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) 
{
    long handle;

    (void)attr; // Attributes not currently used
    if (!rwlock) return EINVAL;
    
    handle = sys_p_thread_sync(THREAD_SYNC_RWLOCK_INIT, 0, 0);
    if (handle < 0) return -handle;
    
    *rwlock = handle;
    return 0;
}
weak_alias (__pthread_rwlock_init, pthread_rwlock_init)


/* =========================== */
/*   pthread_rwlock_destroy    */
/* =========================== */

__typeof__(pthread_rwlock_destroy) __pthread_rwlock_destroy;

int __pthread_rwlock_destroy(pthread_rwlock_t *rwlock) 
{
    long result;
    
    if (!rwlock || *rwlock == 0) return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_RWLOCK_DESTROY, *rwlock, 0);
    if (result < 0) return -result;
    
    *rwlock = 0;
    return 0;
}
weak_alias (__pthread_rwlock_destroy, pthread_rwlock_destroy)


/* =========================== */
/*   pthread_rwlock_rdlock     */
/* =========================== */

__typeof__(pthread_rwlock_rdlock) __pthread_rwlock_rdlock;

int __pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) 
{
    long result;
    
    if (!rwlock || *rwlock == 0) return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_RWLOCK_RDLOCK, *rwlock, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_rwlock_rdlock, pthread_rwlock_rdlock)


/* =========================== */
/* pthread_rwlock_tryrdlock    */
/* =========================== */

__typeof__(pthread_rwlock_tryrdlock) __pthread_rwlock_tryrdlock;

int __pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) 
{
    long result;
    
    if (!rwlock || *rwlock == 0) return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_RWLOCK_TRYRDLOCK, *rwlock, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_rwlock_tryrdlock, pthread_rwlock_tryrdlock)


/* =========================== */
/*   pthread_rwlock_wrlock     */
/* =========================== */

__typeof__(pthread_rwlock_wrlock) __pthread_rwlock_wrlock;

int __pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) 
{
    long result;
    
    if (!rwlock || *rwlock == 0) return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_RWLOCK_WRLOCK, *rwlock, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_rwlock_wrlock, pthread_rwlock_wrlock)


/* =========================== */
/* pthread_rwlock_trywrlock    */
/* =========================== */

__typeof__(pthread_rwlock_trywrlock) __pthread_rwlock_trywrlock;

int __pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) 
{
    long result;
    
    if (!rwlock || *rwlock == 0) return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_RWLOCK_TRYWRLOCK, *rwlock, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_rwlock_trywrlock, pthread_rwlock_trywrlock)


/* =========================== */
/*   pthread_rwlock_unlock     */
/* =========================== */

__typeof__(pthread_rwlock_unlock) __pthread_rwlock_unlock;

int __pthread_rwlock_unlock(pthread_rwlock_t *rwlock) 
{
    long result;
    
    if (!rwlock || *rwlock == 0) return EINVAL;
    
    result = sys_p_thread_sync(THREAD_SYNC_RWLOCK_UNLOCK, *rwlock, 0);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_rwlock_unlock, pthread_rwlock_unlock)