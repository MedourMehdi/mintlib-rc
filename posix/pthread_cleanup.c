#include <mint/mintbind.h>
#include "pthread_internal.h"

void pthread_cleanup_push(void (*routine)(void*), void *arg) {
    trap_1_wlll(P_THREAD_SYNC, THREAD_SYNC_CLEANUP_PUSH, (long)routine, (long)arg);
}

void pthread_cleanup_pop(int execute) {
    void (*routine)(void*);
    void *arg;
    
    long result = trap_1_wlll(P_THREAD_SYNC, THREAD_SYNC_CLEANUP_POP, (long)&routine, (long)&arg);
    
    if (execute && result > 0 && routine) {
        routine(arg);
    }
}