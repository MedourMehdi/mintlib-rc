/*
 * sigtimedwait() - wait for queued signals with timeout
 *
 * This file is part of the MiNTLib project.
 */

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#include "posix/pthread_priv.h"

/* Parameter structure - matches kernel structure */
struct sigtimedwait_params {
    const sigset_t *set;
    siginfo_t *info;
    const struct timespec *timeout;
};

/* =========================== */
/*     sigtimedwait           */
/* =========================== */

__typeof__(sigtimedwait) __sigtimedwait;

/**
 * sigtimedwait - wait for queued signals with timeout
 * @set: set of signals to wait for
 * @info: buffer to receive signal information
 * @timeout: timeout specification (NULL = wait forever)
 *
 * Returns: signal number on success, -1 on error (errno set)
 */
int 
__sigtimedwait(__const sigset_t *set, siginfo_t *info, __const struct timespec *timeout)
{
    long ret;
    struct sigtimedwait_params params;
    
    /* Validate parameters */
    if (!set) {
        __set_errno(EINVAL);
        return -1;
    }
    
    if (timeout) {
        if (timeout->tv_sec < 0 || timeout->tv_nsec < 0 || 
            timeout->tv_nsec >= 1000000000L) {
            __set_errno(EINVAL);
            return -1;
        }
    }
    
    /* Clear info structure if provided */
    if (info) {
        memset(info, 0, sizeof(siginfo_t));
    }
    
    /* Fill parameter structure */
    params.set = set;
    params.info = info;
    params.timeout = timeout;
    
    /* Use the pthread signal syscall wrapper */
    ret = proc_thread_signal(PTSIG_TIMEDWAIT, (long)&params, 0L);
    
    if (ret < 0) {
        __set_errno((int)-ret);
        return -1;
    }
    
    /* ret contains the signal number */
    return (int)ret;
}
weak_alias (__sigtimedwait, sigtimedwait)