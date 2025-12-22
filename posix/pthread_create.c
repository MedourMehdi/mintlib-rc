#include <errno.h>
#include <mint/mintbind.h>
#include <pthread.h>
#include "pthread_priv.h"

short __mint_is_multithreaded = 0;

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg)
{
    long tid;
    
    if (!thread || !start_routine)
        return EINVAL;
    
    tid = Pexec(PE_THREAD, start_routine, arg, (void*)attr);
    
    if (tid < 0) {
        switch (tid) {
            case -ENOMEM: return EAGAIN;
            case -EINVAL: return EINVAL;
            default: return EAGAIN;
        }
    }

    __mint_is_multithreaded = 1; // Set the global flag to indicate multi-threading
    
    *thread = (pthread_t)tid;
    return 0;
}

void pthread_exit(void *retval)
{
    int *errno_ptr;
    
    /* Clean up errno storage before exiting */
    if (__mint_is_multithreaded) {
        errno_ptr = (int *)sys_p_thread_ctrl(THREAD_CTRL_GET_ERRNO_PTR, 0, 0);
        if (errno_ptr != NULL && errno_ptr != (int *)-1) {
            /* Always free and clear - kernel/malloc will handle if it's valid */
            free(errno_ptr);
            sys_p_thread_ctrl(THREAD_CTRL_SET_ERRNO_PTR, 0, 0);
        }
    }

    sys_p_thread_ctrl(THREAD_CTRL_EXIT, (long)retval, 0);
    while(1); // Never returns
}

int pthread_join(pthread_t thread, void **retval)
{
    long result = sys_p_thread_sync(THREAD_SYNC_JOIN, thread, (long)retval);
    if (result < 0) {
        switch (result) {
            case -ESRCH: return ESRCH;
            case -EINVAL: return EINVAL;
            case -EDEADLK: return EDEADLK;
            default: return EINVAL;
        }
    }
    return 0;
}

int pthread_detach(pthread_t thread)
{
    long result = sys_p_thread_ctrl(THREAD_SYNC_DETACH, thread, 0);
    if (result < 0) {
        switch (result) {
            case -ESRCH: return ESRCH;
            case -EINVAL: return EINVAL;
            default: return EINVAL;
        }
    }
    return 0;
}

pthread_t pthread_self(void)
{
    if (__mint_is_multithreaded) {
        return (pthread_t)sys_p_thread_ctrl(THREAD_CTRL_GETID, 0, 0);
    } else {
        return (pthread_t)NULL;
    }
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2;
}

int pthread_yield(void)
{
    return (int)sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
}