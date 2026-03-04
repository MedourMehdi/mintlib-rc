#include <pthread.h>
#include "pthread_priv.h"
#include <errno.h>

/* =========================== */
/*       pthread_cancel        */
/* =========================== */

__typeof__(pthread_cancel) __pthread_cancel;

int __pthread_cancel(pthread_t thread)
{
    long result = sys_p_thread_ctrl(THREAD_CTRL_CANCEL, 
                                   (long)PTHREAD_CANCELED, 
                                   thread);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_cancel, pthread_cancel)


/* =========================== */
/*  pthread_setcancelstate     */
/* =========================== */

__typeof__(pthread_setcancelstate) __pthread_setcancelstate;

int __pthread_setcancelstate(int state, int *oldstate)
{
    long result;
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    result = sys_p_thread_ctrl(THREAD_CTRL_SETCANCELSTATE, 
                                   state, 
                                   (long)oldstate);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_setcancelstate, pthread_setcancelstate)


/* =========================== */
/*  pthread_setcanceltype      */
/* =========================== */

__typeof__(pthread_setcanceltype) __pthread_setcanceltype;

int __pthread_setcanceltype(int type, int *oldtype)
{
    long result;
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    result = sys_p_thread_ctrl(THREAD_CTRL_SETCANCELTYPE, 
                                   type, 
                                   (long)oldtype);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_setcanceltype, pthread_setcanceltype)


/* =========================== */
/*    pthread_testcancel       */
/* =========================== */

__typeof__(pthread_testcancel) __pthread_testcancel;

void __pthread_testcancel(void)
{
    if (!__mint_is_multithreaded) pthread_setup_threading_np();
    sys_p_thread_ctrl(THREAD_CTRL_TESTCANCEL, 0, 0);
}
weak_alias (__pthread_testcancel, pthread_testcancel)