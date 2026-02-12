/* `sysconf', `pathconf', and `confstr' NAME values.  MiNT version.
   Copyright (C) 1993, 1995-1998, 2000, 2001 Free Software Foundation, Inc.
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

#ifndef _UNISTD_H
# error "Never use <bits/confname.h> directly; include <unistd.h> instead."
#endif

/* Values for the NAME argument to `pathconf' and `fpathconf'.  */
#define _PC_LAST	-1	/* max limit # for pathconf - POSIX (!?) */
#define _PC_IOPEN_MAX	0	/* internal limit on open files - NOT POSIX */
#define _PC_LINK_MAX	1	/* max # of links - POSIX */
#define _PC_PATH_MAX	2	/* max len of a full pathname - POSIX */
#define _PC_NAME_MAX	3	/* max len of individual name - POSIX */
#define _PC_PIPE_BUF	4	/* bytes written atomically to fifo - POSIX */
#define _PC_NO_TRUNC	5	/* filename truncation - POSIX */
#define _MINT_PC_NAME_CASE 6	/* case-insensitive file-names */

/* Values for the argument to `sysconf'.  */
#define _SC_LAST			-1	/* max limit # for sysconf - NOT POSIX */
#define _SC_MEMR_MAX			0	/* memory regions per process - NOT POSIX */
#define _SC_ARG_MAX			1	/* max length of cmdln - NOT quite POSIX */
#define _SC_OPEN_MAX			2	/* max # of open files per process - POSIX */
#define _SC_NGROUPS_MAX			3	/* max # supp gids - POSIX */
#define _SC_CHILD_MAX			4	/* max # processes/user - NOT quite POSIX */
#define _SC_CLK_TCK			5	/* clock ticks */
#define _SC_PAGE_SIZE			6	/* pagesize */
#define _SC_PAGESIZE			_SC_PAGE_SIZE	
#define _SC_PHYS_PAGES			7	/* physical pages */
#define _SC_GETPW_R_SIZE_MAX		8 	/* passwd buffer size */
#define _SC_GETGR_R_SIZE_MAX		9 	/* group buffer size */
#define _SC_AVPHYS_PAGES		10	/* available physical pages */
#define _SC_NPROCESSORS_CONF		11 	/* configured processors */
#define _SC_NPROCESSORS_ONLN		12 	/* online processors */
#define _SC_AIO_LISTIO_MAX		13	/* TODO */
#define _SC_AIO_MAX			14	/* TODO */
#define _SC_AIO_PRIO_DELTA_MAX		15	/* TODO */
#define _SC_ATEXIT_MAX			16	/* max # registered functions atexit()/on_exit() */
#define _SC_BC_BASE_MAX			17	/* TODO */
#define _SC_BC_DIM_MAX			18	/* TODO */
#define _SC_BC_SCALE_MAX		19	/* TODO */
#define _SC_BC_STRING_MAX		20	/* TODO */
#define _SC_COLL_WEIGHTS_MAX		21	/* TODO */
#define _SC_DELAYTIMER_MAX		22	/* TODO */
#define _SC_EXPR_NEST_MAX		23	/* TODO */
#define _SC_HOST_NAME_MAX		24	/* hostname buffer size */
#define _SC_IOV_MAX			25	/* size of object which can be written atomically */
#define _SC_LINE_MAX			26	/* TODO */
#define _SC_LOGIN_NAME_MAX		27	/* loginname buffer size */
#define _SC_MQ_OPEN_MAX			28	/* TODO */
#define _SC_MQ_PRIO_MAX			29	/* TODO */
#define _SC_NSIG			30	/* # signals */
#define _SC_THREAD_DESTRUCTOR_ITERATIONS    31	/* TODO */
#define _SC_THREAD_KEYS_MAX		32	/* TODO */
#define _SC_THREAD_STACK_MIN		33	/* TODO */
#define _SC_THREAD_THREADS_MAX		34	/* TODO */
#define _SC_RE_DUP_MAX			35	/* TODO */
#define _SC_RTSIG_MAX			36	/* TODO */
#define _SC_SEM_NSEMS_MAX		37	/* TODO */
#define _SC_SEM_VALUE_MAX		38	/* TODO */
#define _SC_SIGQUEUE_MAX		39	/* TODO */
#define _SC_STREAM_MAX			40	/* max # of open streams per process */
#define _SC_SYMLOOP_MAX			41	/* maximum link count */
#define _SC_TIMER_MAX			42	/* TODO */
#define _SC_TTY_NAME_MAX		43	/* ttyname buffer size */
#define _SC_TZNAME_MAX			44	/* TODO */
#define _SC_ADVISORY_INFO		45	/* Advisory info (posix_fadvise...) supported (1) or not (-1) */
#define _SC_BARRIERS			46	/* TODO */
#define _SC_ASYNCHRONOUS_IO		47	/* asynchronous IO supported (1) or not (-1) */
#define _SC_CLOCK_SELECTION		48	/* TODO */
#define _SC_CPUTIME			49	/* CPU clocks  supported (1) or not (-1) */
#define _SC_DEVICE_CONTROL		50	/* TODO */
#define _SC_FSYNC			51	/* fsync(fh) supported (1) or not (-1) */
#define _SC_IPV6			52	/* IPv6 supported (1) or not (-1) */
#define _SC_JOB_CONTROL			53	/* TODO */
#define _SC_MAPPED_FILES		54	/* memory-mapped files (mmap/munmap/msync) are supported (1) or not (-1) */
#define _SC_MEMLOCK			55	/* TODO */
#define _SC_MEMLOCK_RANGE		56	/* TODO */
#define _SC_MEMORY_PROTECTION		57	/* TODO */
#define _SC_MESSAGE_PASSING		58	/* TODO */
#define _SC_MONOTONIC_CLOCK		59	/* TODO */
#define _SC_PRIORITIZED_IO		60	/* TODO */
#define _SC_PRIORITY_SCHEDULING		61	/* TODO */
#define _SC_RAW_SOCKETS			62	/* TODO */
#define _SC_READER_WRITER_LOCKS		63	/* TODO */
#define _SC_REALTIME_SIGNALS		64	/* # available realtime signals */
#define _SC_REGEXP			65	/* TODO */
#define _SC_SAVED_IDS			66	/* TODO */
#define _SC_SEMAPHORES			67	/* semaphores supported (1) or not (-1) */
#define _SC_SHARED_MEMORY_OBJECTS	68	/* shm_open/shm_unlink supported (1) or not (-1) */
#define _SC_SHELL			69	/* TODO */
#define _SC_SPAWN			70	/* TODO */
#define _SC_SPIN_LOCKS			71	/* spinlocks supported (>0) or not (-1) */
#define _SC_SPORADIC_SERVER		72	/* TODO */
#define _SC_SS_REPL_MAX			73	/* TODO */
#define _SC_SYNCHRONIZED_IO		74	/* TODO */
#define _SC_THREAD_ATTR_STACKADDR	75	/* TODO */
#define _SC_THREAD_ATTR_STACKSIZE	76	/* TODO */
#define _SC_THREAD_CPUTIME		77	/* TODO */
#define _SC_THREAD_PRIO_INHERIT		78	/* TODO */
#define _SC_THREAD_PRIO_PROTECT		79	/* TODO */
#define _SC_THREAD_PRIORITY_SCHEDULING	80	/* TODO */
#define _SC_THREAD_PROCESS_SHARED	81	/* TODO */
#define _SC_THREAD_ROBUST_PRIO_INHERIT	82	/* TODO */
#define _SC_THREAD_ROBUST_PRIO_PROTECT	83	/* TODO */
#define _SC_THREAD_SAFE_FUNCTIONS	84	/* TODO */
#define _SC_THREAD_SPORADIC_SERVER	85	/* TODO */
#define _SC_THREADS			86	/* TODO */
#define _SC_TIMEOUTS			87	/* timeouts for pthread_cond/mq/sem */
#define _SC_TIMERS			88	/* timers supported (>0) or not (-1) */
#define _SC_TYPED_MEMORY_OBJECTS	89	/* typed memory supported (>0) or not (-1) */
#define _SC_VERSION			90	/* TODO */
#define _SC_V8_ILP32_OFF32		91	/* int/long/pointer/off_t are 32bit */
#define _SC_V8_ILP32_OFFBIG		92	/* int/long/pointer are 32bit, off_t is 64bit */
#define _SC_V8_LP64_OFF64		93	/* int is 32bit, long/pointer/off_t are 64bit */
#define _SC_V8_LPBIG_OFFBIG		94	/* int is 32bits, long/pointer/off_t are >64bit */
#define _SC_V7_ILP32_OFF32		95	/* int/long/pointer/off_t are 32bit */
#define _SC_V7_ILP32_OFFBIG		96	/* int/long/pointer are 32bit, off_t is 64bit */
#define _SC_V7_LP64_OFF64		97	/* int is 32bit, long/pointer/off_t are 64bit */
#define _SC_V7_LPBIG_OFFBIG		98	/* int is 32bits, long/pointer/off_t are >64bit */
#define _SC_2_C_BIND			99	/* TODO */
#define _SC_2_C_DEV			100	/* TODO */
#define _SC_2_CHAR_TERM			101	/* TODO */
#define _SC_2_FORT_RUN			102	/* TODO */
#define _SC_2_LOCALEDEF			103	/* TODO */
#define _SC_2_SW_DEV			104	/* TODO */
#define _SC_2_UPE			105	/* TODO */
#define _SC_2_VERSION			106	/* TODO */
#define _SC_XOPEN_CRYPT			107	/* TODO */
#define _SC_XOPEN_ENH_I18N		108	/* TODO */
#define _SC_XOPEN_REALTIME		109	/* TODO */
#define _SC_XOPEN_REALTIME_THREADS	110	/* TODO */
#define _SC_XOPEN_SHM			111	/* TODO */
#define _SC_XOPEN_UNIX			112	/* TODO */
#define _SC_XOPEN_UUCP			113	/* TODO */
#define _SC_XOPEN_VERSION		114	/* TODO */
/* The following values are optional or feature test macro */
#define _SC_2_FORT_DEV			115	/* TODO */
#define _PC_MAX_CANON			116	/* TODO */
#define _PC_MAX_INPUT			117	/* TODO */
#define _PC_CHOWN_RESTRICTED		118	/* TODO */
#define _PC_VDISABLE			119	/* TODO */
#define _PC_SYNC_IO			120	/* TODO */
#define _PC_FILESIZEBITS		121	/* TODO */
/* Update _SC_LAST_NUMBER if new values are added; this value is returned by sysconf(_SC_LAST) */
#define _SC_LAST_NUMBER			121

#if (defined __USE_POSIX2 || defined __USE_UNIX98 \
     || defined __USE_FILE_OFFSET64 || defined __USE_LARGEFILE64 \
     || defined __USE_LARGEFILE)
/* Values for the NAME argument to `confstr'.  */
enum
  {
    _CS_PATH,			/* The default search path.  */
#define _CS_PATH		_CS_PATH

# if (defined __USE_FILE_OFFSET64 || defined __USE_LARGEFILE64 \
     || defined __USE_LARGEFILE)
    _CS_LFS_CFLAGS = 1000,
#  define _CS_LFS_CFLAGS		_CS_LFS_CFLAGS
    _CS_LFS_LDFLAGS,
#  define _CS_LFS_LDFLAGS	_CS_LFS_LDFLAGS
    _CS_LFS_LIBS,
#  define _CS_LFS_LIBS		_CS_LFS_LIBS
    _CS_LFS_LINTFLAGS,
#  define _CS_LFS_LINTFLAGS	_CS_LFS_LINTFLAGS
    _CS_LFS64_CFLAGS,
#  define _CS_LFS64_CFLAGS	_CS_LFS64_CFLAGS
    _CS_LFS64_LDFLAGS,
#  define _CS_LFS64_LDFLAGS	_CS_LFS64_LDFLAGS
    _CS_LFS64_LIBS,
#  define _CS_LFS64_LIBS		_CS_LFS64_LIBS
    _CS_LFS64_LINTFLAGS,
#  define _CS_LFS64_LINTFLAGS	_CS_LFS64_LINTFLAGS
# endif

# ifdef __USE_UNIX98
    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,
#  define _CS_XBS5_ILP32_OFF32_CFLAGS _CS_XBS5_ILP32_OFF32_CFLAGS
    _CS_XBS5_ILP32_OFF32_LDFLAGS,
#  define _CS_XBS5_ILP32_OFF32_LDFLAGS _CS_XBS5_ILP32_OFF32_LDFLAGS
    _CS_XBS5_ILP32_OFF32_LIBS,
#  define _CS_XBS5_ILP32_OFF32_LIBS _CS_XBS5_ILP32_OFF32_LIBS
    _CS_XBS5_ILP32_OFF32_LINTFLAGS,
#  define _CS_XBS5_ILP32_OFF32_LINTFLAGS _CS_XBS5_ILP32_OFF32_LINTFLAGS
    _CS_XBS5_ILP32_OFFBIG_CFLAGS,
#  define _CS_XBS5_ILP32_OFFBIG_CFLAGS _CS_XBS5_ILP32_OFFBIG_CFLAGS
    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,
#  define _CS_XBS5_ILP32_OFFBIG_LDFLAGS _CS_XBS5_ILP32_OFFBIG_LDFLAGS
    _CS_XBS5_ILP32_OFFBIG_LIBS,
#  define _CS_XBS5_ILP32_OFFBIG_LIBS _CS_XBS5_ILP32_OFFBIG_LIBS
    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,
#  define _CS_XBS5_ILP32_OFFBIG_LINTFLAGS _CS_XBS5_ILP32_OFFBIG_LINTFLAGS
    _CS_XBS5_LP64_OFF64_CFLAGS,
#  define _CS_XBS5_LP64_OFF64_CFLAGS _CS_XBS5_LP64_OFF64_CFLAGS
    _CS_XBS5_LP64_OFF64_LDFLAGS,
#  define _CS_XBS5_LP64_OFF64_LDFLAGS _CS_XBS5_LP64_OFF64_LDFLAGS
    _CS_XBS5_LP64_OFF64_LIBS,
#  define _CS_XBS5_LP64_OFF64_LIBS _CS_XBS5_LP64_OFF64_LIBS
    _CS_XBS5_LP64_OFF64_LINTFLAGS,
#  define _CS_XBS5_LP64_OFF64_LINTFLAGS _CS_XBS5_LP64_OFF64_LINTFLAGS
    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,
#  define _CS_XBS5_LPBIG_OFFBIG_CFLAGS _CS_XBS5_LPBIG_OFFBIG_CFLAGS
    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,
#  define _CS_XBS5_LPBIG_OFFBIG_LDFLAGS _CS_XBS5_LPBIG_OFFBIG_LDFLAGS
    _CS_XBS5_LPBIG_OFFBIG_LIBS,
#  define _CS_XBS5_LPBIG_OFFBIG_LIBS _CS_XBS5_LPBIG_OFFBIG_LIBS
    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,
#  define _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS
# endif
# ifdef __USE_XOPEN2K
    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,
#  define _CS_POSIX_V6_ILP32_OFF32_CFLAGS _CS_POSIX_V6_ILP32_OFF32_CFLAGS
    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,
#  define _CS_POSIX_V6_ILP32_OFF32_LDFLAGS _CS_POSIX_V6_ILP32_OFF32_LDFLAGS
    _CS_POSIX_V6_ILP32_OFF32_LIBS,
#  define _CS_POSIX_V6_ILP32_OFF32_LIBS _CS_POSIX_V6_ILP32_OFF32_LIBS
    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,
#  define _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS
    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,
#  define _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS
    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,
#  define _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS
    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,
#  define _CS_POSIX_V6_ILP32_OFFBIG_LIBS _CS_POSIX_V6_ILP32_OFFBIG_LIBS
    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,
#  define _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS
    _CS_POSIX_V6_LP64_OFF64_CFLAGS,
#  define _CS_POSIX_V6_LP64_OFF64_CFLAGS _CS_POSIX_V6_LP64_OFF64_CFLAGS
    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,
#  define _CS_POSIX_V6_LP64_OFF64_LDFLAGS _CS_POSIX_V6_LP64_OFF64_LDFLAGS
    _CS_POSIX_V6_LP64_OFF64_LIBS,
#  define _CS_POSIX_V6_LP64_OFF64_LIBS _CS_POSIX_V6_LP64_OFF64_LIBS
    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,
#  define _CS_POSIX_V6_LP64_OFF64_LINTFLAGS _CS_POSIX_V6_LP64_OFF64_LINTFLAGS
    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,
#  define _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS
    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,
#  define _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS
    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,
#  define _CS_POSIX_V6_LPBIG_OFFBIG_LIBS _CS_POSIX_V6_LPBIG_OFFBIG_LIBS
    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,
#  define _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS
# endif

    _CS_V6_WIDTH_RESTRICTED_ENVS
# define _CS_V6_WIDTH_RESTRICTED_ENVS	_CS_V6_WIDTH_RESTRICTED_ENVS
  };
#endif
