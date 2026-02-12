/*	$NetBSD: mman.h,v 1.66 2025/11/15 19:02:26 gutteridge Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mman.h	8.2 (Berkeley) 1/9/95
 */

/* Modified for MiNTLib, Feb 12 2026.  */

#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_

#include <features.h>
#include <sys/cdefs.h>
#include <stddef.h> /* for size_t */
#include <sys/types.h> /* for mode_t and off_t */

/*
 * Protection options
 */
#define	PROT_NONE	0x00	/* no permissions */
#define	PROT_READ	0x01	/* pages can be read */
#define	PROT_WRITE	0x02	/* pages can be written */
#define	PROT_EXEC	0x04	/* pages can be executed */

/*
 * Flags options
 */
#define	MAP_SHARED	0x0001	/* share changes */
#define	MAP_PRIVATE	0x0002	/* changes are private */
#define	MAP_FIXED	0x0004	/* map addr must be exactly as requested */

/*
 * msync() function
 */
#define	MS_ASYNC	0x01	/* perform asynchronous writes */
#define	MS_INVALIDATE	0x02	/* invalidate cached data */
#define	MS_SYNC		0x04	/* perform synchronous writes */

/*
 * mlockall() function
 */
#define	MCL_CURRENT	0x01	/* lock all pages currently mapped */
#define	MCL_FUTURE	0x02	/* lock all pages mapped in the future */

/*
 * Error indicator returned by mmap(2)
 */
#define	MAP_FAILED	((void *) -1)	/* mmap() failed */

/*
 * POSIX memory advisory values.
 */
#define	POSIX_MADV_NORMAL	0	/* No further special treatment */
#define	POSIX_MADV_RANDOM	1	/* Expect random page references */
#define	POSIX_MADV_SEQUENTIAL	2	/* Expect sequential page references */
#define	POSIX_MADV_WILLNEED	3	/* Will need these pages */
#define	POSIX_MADV_DONTNEED	4	/* Don't need these pages */

/*
 * POSIX posix_typed_mem_open function flags.
 */
#define	POSIX_TYPED_MEM_ALLOCATE	0	/* Allocate on mmap */
#define	POSIX_TYPED_MEM_ALLOCATE_CONTIG	1	/* Allocate contiguously on mmap */
#define	POSIX_TYPED_MEM_MAP_ALLOCATABLE	2	/* Map on mmap without affecting allocatability */

__BEGIN_DECLS

void *	mmap(void *, size_t, int, int, int, off_t);
int	munmap(void *, size_t);
int	mprotect(void *, size_t, int);
int	msync(void *, size_t, int) __RENAME(__msync13);
int	mlock(const void *, size_t);
int	munlock(const void *, size_t);
int	mlockall(int);
int	munlockall(void);
int	posix_madvise(void *, size_t, int);
int	posix_posix_mem_offset(const void *restrict, size_t, off_t *restrict, 
		size_t *restrict, int *restrict);
int	posix_typed_mem_get_info(int, struct posix_typed_mem_info *);
int	posix_typed_mem_open(const char *, int, int);
int	shm_open(const char *, int, mode_t);
int	shm_unlink(const char *);

__END_DECLS

#endif /* !_SYS_MMAN_H_ */