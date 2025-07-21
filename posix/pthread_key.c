#include <errno.h>
#include <pthread.h>
#include "pthread_priv.h"

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    long result;
        
    if (!key) return EINVAL;
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
    long expected, desired;
    long *once_ptr;
    
    if (!once_control || !init_routine) return EINVAL;
    
    /* Treat once_control as a pointer to a long */
    once_ptr = (long*)once_control;
    expected = 0;  /* We expect the value to be 0 (not initialized) */
    desired = 1;   /* We want to set it to 1 (initialized) */
    
    do {
        /* Atomically check if *once_ptr is 0 and set to 1 if true */
        /* CAS returns 1 on success, 0 on failure */
        if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)once_ptr, expected, desired) == 1) {
            /* We successfully changed the value from 0 to 1, so we run the init */
            init_routine();
            break;
        }
        
        /* Check if initialization is already complete */
        if (*(volatile long*)once_ptr == 1) {
            /* Another thread completed initialization */
            break;
        }
        
        /* Yield if CAS fails (another thread is initializing) */
        sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
    } while (1);
    
    return 0;
}