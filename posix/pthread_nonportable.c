#include <mint/mintbind.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> /* for usleep */

#include "pthread_priv.h"

int pthread_setname_np(pthread_t thread, const char *name) {
    long result;
    size_t len = 0;
    if (!name) return EINVAL;
    
    // Limit name to 15 characters + null terminator
    len = strnlen(name, 16);
    if (len >= 16) return ERANGE;
    
    result = sys_p_thread_ctrl(THREAD_CTRL_SETNAME, thread, (long)name);
    return (result < 0) ? -result : 0;
}

int pthread_getname_np(pthread_t thread, char *name, size_t len) {
    long result;
    if (!name || len == 0) return EINVAL;
    if (len < 16) return ERANGE;  // Need space for 15 chars + null
    
    result = sys_p_thread_ctrl(THREAD_CTRL_GETNAME, thread, (long)name);
    return (result < 0) ? -result : 0;
}

int pthread_is_initialthread_np(void) {
    long result = sys_p_thread_ctrl(THREAD_CTRL_IS_INITIAL, 0, 0);
    return (result != 0);
}

int pthread_is_multithreaded_np(void) {
    long result = sys_p_thread_ctrl(THREAD_CTRL_IS_MULTITHREADED, 0, 0);
    return (result != 0);
}

int msleep(long ms) {
    if ( __mint_is_multithreaded && ((long)sys_p_thread_ctrl(THREAD_CTRL_GETID, 0, 0)) > 0) {
        long result = sys_p_thread_sync(THREAD_SYNC_SLEEP, ms, 0);
        return (result < 0) ? -result : 0;
    }
    return usleep(ms * 1000);
}

/*
 * pthread_tryjoin_np - attempt to join with a thread without blocking
 * Non-portable extension for MiNT pthread implementation
 */
int pthread_tryjoin_np(pthread_t thread, void **retval)
{
    long result = 1;
    while (result != 0) {
        result = sys_p_thread_sync(THREAD_SYNC_TRYJOIN, thread, (long)retval);
        pthread_yield();
        // msleep(10); // Sleep for 10ms before checking again
    }
    return 0; // Successfully joined
}
