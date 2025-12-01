/*
 * sigqueue() - POSIX real-time signal with data
 *
 * This file is part of the MiNTLib project.
 */

#include <signal.h>
#include <errno.h>
#include <sys/types.h>

#include "posix/pthread_priv.h"
#include <stdio.h>
/* Parameter structure - matches kernel structure */
struct sigqueue_params {
	long pid;      /* Use internal type for consistency */
	long sig;
	union sigval value;
};

/**
 * sigqueue - queue a signal and data to a process
 * @pid: target process ID
 * @sig: signal number to send
 * @value: signal value (integer or pointer)
 *
 * Returns: 0 on success, -1 on error (errno set)
 */
int 
sigqueue(__pid_t pid, int sig, __const union sigval value)
{
	long ret;
	struct sigqueue_params params;
	
	/* Validate parameters */
	if (sig < 0 || sig >= NSIG) {
		__set_errno(EINVAL);
		return -1;
	}
	
	if (pid < 0) {
		__set_errno(EINVAL);
		return -1;
	}
	
	/* Fill parameter structure */
	params.pid = (long)pid;
	params.sig = sig;
	params.value = value;
printf("Before syscall: &params=%p, params.pid=%ld, params.sig=%ld, params.value=%ld\n",
       &params, params.pid, params.sig, params.value.sival_int);
    /* Use the pthread signal syscall wrapper */
    ret = proc_thread_signal(PTSIG_QUEUE, (long)&params, 0L);
	if (ret < 0) {
		__set_errno((int)-ret);
		return -1;
	}
	
	return 0;
}