#include <mint/mintbind.h>
#include "pthread_priv.h"

void pthread_cleanup_push(void (*routine)(void*), void *arg) {
    sys_p_thread_sync(THREAD_SYNC_CLEANUP_PUSH, (long)routine, (long)arg);
}

/**
 * Removes the top cleanup handler from the stack and optionally executes it.
 *
 * If the `execute` parameter is non-zero, the function pointed to by the cleanup
 * handler is called with the argument specified when the handler was pushed.
 *
 * @param execute Non-zero to execute the cleanup handler, zero to only remove it.
 */

void pthread_cleanup_pop(int execute) {
    void (*routine)(void*);
    void *arg;
    
    long result = sys_p_thread_sync(THREAD_SYNC_CLEANUP_POP, (long)&routine, (long)&arg);
    
    if (execute && result > 0 && routine) {
        routine(arg);
    }
}