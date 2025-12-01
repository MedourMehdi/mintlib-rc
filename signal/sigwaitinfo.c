/*
 * sigwaitinfo() - wait for queued signals
 *
 * This file is part of the MiNTLib project.
 */

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "posix/pthread_priv.h"

/**
 * sigwaitinfo - wait for queued signals
 * @set: set of signals to wait for
 * @info: buffer to receive signal information
 *
 * Returns: signal number on success, -1 on error (errno set)
 */
int 
sigwaitinfo(__const sigset_t *set, siginfo_t *info)
{
	long ret;
	
	/* Validate parameters */
	if (!set) {
		__set_errno(EINVAL);
		return -1;
	}
	
	/* Clear info structure if provided */
	if (info) {
		memset(info, 0, sizeof(siginfo_t));
	}
	
    /* Use the pthread signal syscall wrapper */
    ret = proc_thread_signal(PTSIG_WAITINFO, (long)set, (long)info);
	
	if (ret < 0) {
		__set_errno((int)-ret);
		return -1;
	}
	
	/* ret contains the signal number */
	return (int)ret;
}

/* Weak alias for compatibility */
#ifdef __ELF__
__asm__(".weak __sigwaitinfo; __sigwaitinfo = sigwaitinfo");
#endif