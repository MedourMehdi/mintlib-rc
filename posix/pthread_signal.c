/*
 * pthread_signal.c - Thread-aware signal handling for MiNT
 *
 * This file provides thread-specific implementations of signal
 * management functions, adapting their behavior based on whether
 * the program is running in a multithreaded environment or not.
 *
 * In classic (single-threaded) mode, the functions delegate to
 * traditional process-level signal handling functions. In threaded
 * mode, they utilize per-thread signal masks and operations.
 *
 * Note: Some functions are non-standard MiNT extensions and may not
 * be available in all POSIX-compliant systems.
 */
#include <pthread.h>
#include "pthread_priv.h"
#include <errno.h>
#include <signal.h>

/* Macro to switch between threaded and classic implementations */
#define PTHREAD_SIGNAL_MODE_SWITCH(threaded_code, classic_code) \
    do { \
        if (__mint_is_multithreaded) { \
            threaded_code; \
        } else { \
            classic_code; \
        } \
    } while(0)


/* =========================== */
/*     pthread_sigmask         */
/* =========================== */

__typeof__(pthread_sigmask) __pthread_sigmask;

/* 
 * pthread_sigmask - Thread-specific signal mask management
 * In classic mode: delegates to Psigsetmask/Psigblock (unchanged)
 * In threaded mode: uses per-thread signal masks
 */
int __pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Use kernel's thread-specific signal masks */
        {
            long old_mask = proc_thread_signal(PTSIG_GETMASK, 0, 0);
            if (oldset) *oldset = (sigset_t)old_mask;
            
            if (set) {
                switch (how) {
                    case SIG_BLOCK:   return proc_thread_signal(PTSIG_BLOCK, *set, 0);
                    case SIG_UNBLOCK: return proc_thread_signal(PTSIG_UNBLOCK, *set, 0);
                    case SIG_SETMASK: return proc_thread_signal(PTSIG_SETMASK, *set, 0);
                    default:          return EINVAL;
                }
            }
            return 0;
        },
        
        /* CLASSIC PATH: Use traditional process-level signal masks */
        {
            long old_mask = Psigsetmask(0); /* Get current mask */
            if (oldset) *oldset = (sigset_t)(old_mask & ~1L); /* Mask out SIGNULL */
            
            if (set) {
                long new_mask = *set & ~1L; /* Ensure SIGNULL not masked */
                switch (how) {
                    case SIG_BLOCK:   Psigblock(new_mask); break;
                    case SIG_UNBLOCK: Psigsetmask(old_mask & ~new_mask); break;
                    case SIG_SETMASK: Psigsetmask(new_mask); break;
                }
            }
            return 0;
        }
    );
}
weak_alias (__pthread_sigmask, pthread_sigmask)


/* =========================== */
/*       pthread_kill          */
/* =========================== */

__typeof__(pthread_kill) __pthread_kill;

/* 
 * pthread_kill - Send signal to specific thread or process
 * In classic mode: Treats thread ID as process ID for compatibility
 * In threaded mode: Delivers to specific thread within process
 */
int __pthread_kill(pthread_t thread, int sig)
{
    int result;
    
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Deliver signal to specific thread */
        {
            long ret = proc_thread_signal(PTSIG_KILL, (long)thread, sig);
            result = (ret < 0) ? -ret : ret;
        },
        
        /* CLASSIC PATH: Use Pkill() for process-level signals */
        {
            long ret = Pkill(Pgetpid(), sig);
            result = (ret < 0) ? -ret : 0;
        }
    );
    
    return result;
}
weak_alias (__pthread_kill, pthread_kill)


/* =========================== */
/*     pthread_sigwait         */
/* =========================== */

__typeof__(pthread_sigwait) __pthread_sigwait;

/* 
 * pthread_sigwait - Synchronously wait for signals (blocking)
 * In classic mode: Uses standard sigwait() implementation (from sigwait.c)
 * In threaded mode: Thread-aware blocking wait
 */
int __pthread_sigwait(const sigset_t *set, int *sig)
{
    if (!set || !sig) return EINVAL;
    
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Thread-specific blocking wait */
        {
            long result = proc_thread_signal(PTSIG_WAIT, *set, -1);
            if (result > 0) {
                *sig = result;
                return 0;
            }
            return -result;
        },
        
        /* CLASSIC PATH: Use standard sigwait() (unchanged) */
        {
            return sigwait(set, sig);
        }
    );
}
weak_alias (__pthread_sigwait, pthread_sigwait)


/* =========================== */
/*  pthread_sigtimedwait       */
/* =========================== */

__typeof__(pthread_sigtimedwait) __pthread_sigtimedwait;

/* 
 * pthread_sigtimedwait - Wait for signals with timeout
 * In classic mode: Uses standard sigtimedwait() (from sigtimedwait.c)
 * In threaded mode: Thread-aware timed wait
 */
int __pthread_sigtimedwait(const sigset_t *set, int *sig, long timeout)
{
    if (!set || !sig) return EINVAL;
    
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Thread-specific timed wait */
        {
            long result = proc_thread_signal(PTSIG_WAIT, *set, timeout);
            if (result > 0) {
                *sig = result;
                return 0;
            }
            return -result;
        },
        
        /* CLASSIC PATH: Use standard sigtimedwait() (unchanged) */
        {
            /* Convert milliseconds timeout to timespec */
            struct timespec ts;
            int ret;
            ts.tv_sec = timeout / 1000;
            ts.tv_nsec = (timeout % 1000) * 1000000;

            ret = sigtimedwait(set, NULL, &ts);
            if (ret > 0) {
                *sig = ret;
                return 0;
            }
            return errno;
        }
    );
}
weak_alias (__pthread_sigtimedwait, pthread_sigtimedwait)


/* =========================== */
/*    pthread_kill_all         */
/* =========================== */

__typeof__(pthread_kill_all) __pthread_kill_all;

/* 
 * pthread_kill_all - Broadcast signal to all threads in process
 * WARNING: Non-standard MiNT extension
 * In classic mode: Sends signal to entire process group (similar effect)
 */
int __pthread_kill_all(int sig)
{
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Broadcast to all threads */
        {
            long ret = proc_thread_signal(PTSIG_BROADCAST, sig, 0);
            return (ret < 0) ? -ret : ret;
        },
        
        /* CLASSIC PATH: Send to process group (0 = current pgrp) */
        {
            long ret = Pkill(0, sig);
            return (ret < 0) ? -ret : 0;
        }
    );
}
weak_alias (__pthread_kill_all, pthread_kill_all)


/* =========================== */
/*   pthread_sigpending        */
/* =========================== */

__typeof__(pthread_sigpending) __pthread_sigpending;

/* 
 * pthread_sigpending - Get pending signals for current thread/process
 * WARNING: Non-standard extension (POSIX sigpending() is process-level only)
 * In classic mode: Uses standard sigpending()
 */
int __pthread_sigpending(sigset_t *set)
{
    if (!set) return EINVAL;
    
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Get thread-specific pending signals */
        {
            long pending = proc_thread_signal(PTSIG_PENDING, 0, 0);
            *set = (sigset_t)pending;
            return 0;
        },
        
        /* CLASSIC PATH: Use standard sigpending() (unchanged) */
        {
            return sigpending(set);
        }
    );
}
weak_alias (__pthread_sigpending, pthread_sigpending)


/* =========================== */
/*    pthread_sigqueue         */
/* =========================== */

__typeof__(pthread_sigqueue) __pthread_sigqueue;

/* 
 * pthread_sigqueue - Queue signal with data to specific thread
 * WARNING: Non-standard extension (POSIX sigqueue() is process-level only)
 * In classic mode: Uses standard sigqueue() (ignores thread ID)
 */
int __pthread_sigqueue(pthread_t thread, int sig, const union sigval value)
{
    if (__mint_is_multithreaded) {
        /* THREADED PATH: Queue to specific thread */
    
        struct {
            pthread_t thread;
            int sig;
            union sigval value;
        } args = { thread, sig, value };
        
        long ret = proc_thread_signal(PTSIG_QUEUE, (long)&args, 0);
        return (ret < 0) ? -ret : ret;
    } else {
        /* CLASSIC PATH: Use standard sigqueue() (unchanged) */
        /* In classic mode, thread parameter is treated as process ID */
        long ret = sigqueue((pid_t)thread, sig, value);
        return (ret < 0) ? -ret : ret;
    }
}
weak_alias (__pthread_sigqueue, pthread_sigqueue)


/* =========================== */
/*    pthread_sigpause         */
/* =========================== */

__typeof__(pthread_sigpause) __pthread_sigpause;

/* 
 * pthread_sigpause - Atomically set mask and pause until signal
 * WARNING: Non-standard MiNT extension
 * In classic mode: Uses Psigsetmask() + Pause()
 */
int __pthread_sigpause(unsigned long mask)
{
    PTHREAD_SIGNAL_MODE_SWITCH(
        /* THREADED PATH: Thread-aware pause */
        {
            long ret = proc_thread_signal(PTSIG_PAUSE, mask, 0);
            return (ret < 0) ? -ret : ret;
        },
        
        /* CLASSIC PATH: Standard signal pause */
        {
            long old = Psigsetmask(mask & ~1L);
            Pause();
            Psigsetmask(old);
            return 0;
        }
    );
}
weak_alias (__pthread_sigpause, pthread_sigpause)