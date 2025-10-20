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


/* Maximum number of threads we support for errno storage */
#define MAX_ERRNO_THREADS 256

/* Internal errno support */
int *__errno_location(void);

/* Single-threaded errno for when threading is not active */
static int __errno_storage = 0;

/* Thread-specific errno array */
typedef struct {
    pthread_t tid;
    int errno_val;
    int in_use;
} errno_entry_t;

static errno_entry_t errno_table[MAX_ERRNO_THREADS];
static int errno_table_initialized = 0;

/* Initialize the errno table (called once when first thread is created) */
static void init_errno_table(void)
{
    if (!errno_table_initialized) {
        memset(errno_table, 0, sizeof(errno_table));
        errno_table_initialized = 1;
    }
}

/* Get the address of the current thread's errno */
int *__errno_location(void)
{
    pthread_t self;
    int i;
    
    /* If not multithreaded, use static storage */
    if (!__mint_is_multithreaded) {
        return &__errno_storage;
    }
    
    /* Initialize table if needed */
    if (!errno_table_initialized) {
        init_errno_table();
    }
    
    /* Get current thread ID */
    self = pthread_self();
    
    /* Find existing entry for this thread */
    for (i = 0; i < MAX_ERRNO_THREADS; i++) {
        if (errno_table[i].in_use && errno_table[i].tid == self) {
            return &errno_table[i].errno_val;
        }
    }
    
    /* Allocate new entry for this thread */
    for (i = 0; i < MAX_ERRNO_THREADS; i++) {
        if (!errno_table[i].in_use) {
            errno_table[i].tid = self;
            errno_table[i].errno_val = 0;
            errno_table[i].in_use = 1;
            return &errno_table[i].errno_val;
        }
    }
    
    /* Table full - fallback to static storage (not ideal) */
    return &__errno_storage;
}

/* Clean up errno entry when thread exits */
void __errno_thread_cleanup(pthread_t tid)
{
    int i;
    
    if (!errno_table_initialized) {
        return;
    }
    
    for (i = 0; i < MAX_ERRNO_THREADS; i++) {
        if (errno_table[i].in_use && errno_table[i].tid == tid) {
            errno_table[i].in_use = 0;
            errno_table[i].tid = 0;
            errno_table[i].errno_val = 0;
            break;
        }
    }
}