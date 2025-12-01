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

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include "pthread.h"
#include "posix/pthread_priv.h"

__typeof__(sigwait) __sigwait;

int __sigwait(const sigset_t *set, int *sig)
{
    int result;
    
    if (!set || !sig) {
        __set_errno(EINVAL);
        return -1;
    }
    
    /* Use sigwaitinfo which is already thread-aware in your kernel */
    result = sigwaitinfo(set, NULL);
    
    if (result < 0) {
        /* errno already set by sigwaitinfo */
        return -1;
    }
    
    *sig = result;
    return 0;
}

weak_alias(__sigwait, sigwait)
