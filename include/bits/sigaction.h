/* The proper definitions for FreeMiNT's sigaction.
   Copyright (C) 1993-1999, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

/* Modified for MiNTLib by Frank Naumann <fnaumann@freemint.de>.  */

#ifndef _SIGNAL_H
# error "Never include <bits/sigaction.h> directly; use <signal.h> instead."
#endif

/* values for "how" parameter to sigprocmask() */
#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2

#define SA_NOCLDSTOP   0x0001       /* don't send SIGCHLD when they stop */
#define SA_NODEFER     0x0010       /* don't mask the signal we're delivering */
#define SA_RESETHAND   0x8000       /* Reset to SIG_DFL on entry to handler. */

#ifdef __USE_MISC
/* Some aliases for the SA_ constants.  */
# define SA_NOMASK    SA_NODEFER
# define SA_ONESHOT   SA_RESETHAND
#endif

/* SA_SIGINFO already defined in <bits/siginfo.h>, included earlier via
   <signal.h>. Left here only as a comment for reference:
     SA_SIGINFO   0x0040
   SA_NOCLDWAIT/SA_ONSTACK/SA_RESTART remain NYI kernel-side; not defined
   here to avoid callers believing they're functional. */

struct sigaction {
	union {
		__sighandler_t sa_handler;	/* Pointer to signal handler.  */
		void (*sa_sigaction) (int, siginfo_t *, void *);
						/* Pointer to SA_SIGINFO handler.  */
	};
	__sigset_t	sa_mask;	/* Additional signals masked during
					   delivery.  */
	int		sa_flags;	/* Signal specific flags.  */
};