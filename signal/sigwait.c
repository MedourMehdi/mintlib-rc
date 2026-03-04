/* Implementation of sigwait function from POSIX.1c.
   Copyright (C) 1996, 1997, 1999, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

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

/* =========================== */
/*      sigwaitinfo           */
/* =========================== */

__typeof__(sigwaitinfo) __sigwaitinfo;

/**
 * sigwaitinfo - wait for queued signals
 * @set: set of signals to wait for
 * @info: buffer to receive signal information
 *
 * Returns: signal number on success, -1 on error (errno set)
 */
int 
__sigwaitinfo(__const sigset_t *set, siginfo_t *info)
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
weak_alias (__sigwaitinfo, sigwaitinfo)