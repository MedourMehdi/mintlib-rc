#include <mint/mintbind.h>
#include <errno.h>
#include <string.h>
#include "pthread_priv.h"

int pthread_setname_np(pthread_t thread, const char *name) {
    long result;
    size_t len = 0;
    if (!name) return EINVAL;
    
    // Limit name to 15 characters + null terminator
    len = strnlen(name, 16);
    if (len >= 16) return ERANGE;
    
    result = trap_1_wlll(P_THREAD_CTRL, THREAD_CTRL_SETNAME, thread, (long)name);
    return (result < 0) ? -result : 0;
}

int pthread_getname_np(pthread_t thread, char *name, size_t len) {
    long result;
    if (!name || len == 0) return EINVAL;
    if (len < 16) return ERANGE;  // Need space for 15 chars + null
    
    result = trap_1_wlll(P_THREAD_CTRL, THREAD_CTRL_GETNAME, thread, (long)name);
    return (result < 0) ? -result : 0;
}

int pthread_is_initialthread_np(void) {
    long result = trap_1_wlll(P_THREAD_CTRL, THREAD_CTRL_IS_INITIAL, 0, 0);
    return (result != 0);
}

int pthread_is_multithreaded_np(void) {
    long result = trap_1_wlll(P_THREAD_CTRL, THREAD_CTRL_IS_MULTITHREADED, 0, 0);
    return (result != 0);
}

int msleep(long ms) {
    long result = trap_1_wlll(P_THREAD_SYNC, THREAD_SYNC_SLEEP, ms, 0);
    return (result < 0) ? -result : 0;
}

int pthread_tryjoin_np(pthread_t thread, void **retval)
{
    while (1) {
        long status = proc_thread_status(thread);
        if (status == ESRCH) {
            return (status == ESRCH) ? ESRCH : EINVAL;
        }
        if (status & THREAD_STATE_EXITED) {
            // Thread exited, now do the actual join to get return value
            long result = sys_p_thread_sync(THREAD_SYNC_TRYJOIN, thread, (long)retval);
            if (result == 0 || result != EAGAIN) {
                return (result == 0) ? 0 : EINVAL;
            }
        }
        // pthread_yield();
        msleep(1000); // Sleep for 1000ms before checking again
    }
}

/*
 * pthread_tryjoin_np - attempt to join with a thread without blocking
 * Non-portable extension for MiNT pthread implementation
 */

// int pthread_tryjoin_np(pthread_t thread, void **retval)
// {
//     long result;
//     long status;

//     /* Validate thread parameter */
//     if (thread <= 0) {
//         return EINVAL;
//     }

//     /* Check if trying to join with self */
//     if (thread == pthread_self()) {
//         return EDEADLK;
//     }

//     /* First check the thread status to see if it's joinable */
//     status = proc_thread_status(thread);
//     if (status < 0) {
//         /* Thread doesn't exist or invalid */
//         return ESRCH;
//     }

//     /* Check if thread has already exited */
//     if (!(status & THREAD_STATE_EXITED)) {
//         /* Thread is still running, return immediately without blocking */
//         return EBUSY;
//     }

//     /* Thread has exited, attempt to join */
//     result = sys_p_thread_sync(THREAD_SYNC_TRYJOIN, thread, (long)retval);
    
//     if (result < 0) {
//         switch (-result) {
//             case ESRCH:
//                 /* Thread not found */
//                 return ESRCH;
//             case EINVAL:
//                 /* Thread is not joinable (detached) */
//                 return EINVAL;
//             case EDEADLK:
//                 /* Deadlock detected */
//                 return EDEADLK;
//             case EBUSY:
//                 /* Thread not ready for joining yet */
//                 return EBUSY;
//             default:
//                 return -result;
//         }
//     }

//     /* Success */
//     return 0;
// }