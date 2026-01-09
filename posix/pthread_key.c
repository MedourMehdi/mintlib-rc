#include <errno.h>
#include <pthread.h>
#include "pthread_priv.h"
#include <stdio.h>

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    long result;

    if (!key) return EINVAL;
    if (!__mint_is_multithreaded) {
        pthread_setup_threading_np();
    }    
    result = sys_p_thread_tsd(THREAD_TSD_CREATE_KEY, (long)destructor, 0);
    if (result < 0) return -result;
    *key = (pthread_key_t)result;
    return 0;
}

int pthread_key_delete(pthread_key_t key)
{
    long result = sys_p_thread_tsd(THREAD_TSD_DELETE_KEY, key, 0);
    if (result < 0) return -result;
    return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
    return (void*)sys_p_thread_tsd(THREAD_TSD_GET_SPECIFIC, key, 0);
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    long result = sys_p_thread_tsd(THREAD_TSD_SET_SPECIFIC, key, (long)value);
    if (result < 0) return -result;
    return 0;
}


int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    long *once_ptr = (long*)once_control;
    
    // Try to win the race
    if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)once_ptr, 0, 1) == 1) {
        init_routine();
        // No need to set state - CAS already set it to 1
        return 0;
    }
    
    // Lost race - wait for completion
    while (*(volatile long*)once_ptr != 1) {
        sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
    }
    
    return 0;
}
