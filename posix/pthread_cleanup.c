#include <mint/mintbind.h>
#include "pthread_priv.h"

/* =========================== */
/*    pthread_cleanup_push     */
/* =========================== */

__typeof__(pthread_cleanup_push) __pthread_cleanup_push;

void __pthread_cleanup_push(void (*routine)(void*), void *arg) 
{
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    sys_p_thread_sync(THREAD_SYNC_CLEANUP_PUSH, (long)routine, (long)arg);
}
weak_alias (__pthread_cleanup_push, pthread_cleanup_push)


/* =========================== */
/*    pthread_cleanup_pop      */
/* =========================== */

__typeof__(pthread_cleanup_pop) __pthread_cleanup_pop;

/**
 * Removes the top cleanup handler from the stack and optionally executes it.
 *
 * If the `execute` parameter is non-zero, the function pointed to by the cleanup
 * handler is called with the argument specified when the handler was pushed.
 *
 * @param execute Non-zero to execute the cleanup handler, zero to only remove it.
 */
void __pthread_cleanup_pop(int execute) 
{
    void (*routine)(void*);
    void *arg;
    long result;
    
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    result = sys_p_thread_sync(THREAD_SYNC_CLEANUP_POP, (long)&routine, (long)&arg);
    
    if (execute && result > 0 && routine) {
        routine(arg);
    }
}
weak_alias (__pthread_cleanup_pop, pthread_cleanup_pop)