#include <pthread.h>
#include "pthread_internal.h"
#include <errno.h>

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
    long old_mask = 0;
    
    if (oldset) {
        old_mask = proc_thread_signal(PTSIG_GETMASK, 0, 0);
        *oldset = old_mask;
    }
    
    if (!set) return 0;
    
    switch (how) {
        case SIG_BLOCK:
            return proc_thread_signal(PTSIG_BLOCK, *set, 0);
        case SIG_UNBLOCK:
            return proc_thread_signal(PTSIG_UNBLOCK, *set, 0);
        case SIG_SETMASK:
            return proc_thread_signal(PTSIG_SETMASK, *set, 0);
        default:
            return EINVAL;
    }
}

int pthread_kill(pthread_t thread, int sig)
{
    if (thread <= 0 || sig < 0) return EINVAL;
    return proc_thread_signal(PTSIG_KILL, thread, sig);
}

int pthread_sigwait(const sigset_t *set, int *sig)
{
    long result;

    if (!set || !sig) return EINVAL;
    
    result = proc_thread_signal(PTSIG_WAIT, *set, -1);
    if (result > 0) {
        *sig = result;
        return 0;
    }
    return -result;
}

int pthread_sigtimedwait(const sigset_t *set, int *sig, long timeout)
{
    long result;

    if (!set || !sig) return EINVAL;
    
    result = proc_thread_signal(PTSIG_WAIT, *set, timeout);
    if (result > 0) {
        *sig = result;
        return 0;
    }
    return -result;
}

int pthread_kill_all(int sig)
{
    return proc_thread_signal(PTSIG_BROADCAST, sig, 0);
}