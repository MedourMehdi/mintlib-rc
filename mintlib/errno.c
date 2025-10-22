/*  globals.c -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

/* This file defines various internal library variables, mainly for
   the purpose that main.c doesn't define them.  Otherwise it will
   become very hard to override _main() in special programs such
   as drivers.  */
   
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "posix/pthread_priv.h"

/* Single-threaded errno for when threading is not active */
static int __errno_storage = 0;

/* Get the address of the current thread's errno */
int *__errno_location(void)
{
    int *errno_ptr;
    int *new_errno;
    long result;
    
    /* If not multithreaded, use static storage */
    if (!__mint_is_multithreaded) {
        return &__errno_storage;
    }
    
    /* Ask kernel for current thread's errno pointer */
    errno_ptr = (int *)sys_p_thread_ctrl(THREAD_CTRL_GET_ERRNO_PTR, 0, 0);
    
    /* If already set, return it */
    if (errno_ptr != NULL && errno_ptr != (int *)-1) {
        return errno_ptr;
    }
    
    /* First access for this thread - allocate new errno storage */
    new_errno = (int *)malloc(sizeof(int));
    if (new_errno == NULL) {
        /* Allocation failed - fallback to static (not thread-safe!) */
        return &__errno_storage;
    }
    
    /* Initialize to 0 */
    *new_errno = 0;
    
    /* Tell kernel about our errno pointer */
    result = sys_p_thread_ctrl(THREAD_CTRL_SET_ERRNO_PTR, (long)new_errno, 0);
    if (result < 0) {
        /* Setting failed - free and fallback */
        free(new_errno);
        return &__errno_storage;
    }
    
    return new_errno;
}

/* Helper function for assembly code to set errno */
/* Assembly code cannot use the errno macro, so this wrapper allows */
/* assembly to set errno in a thread-safe way */
void __set_errno_asm(int err)
{
    errno = err;
}