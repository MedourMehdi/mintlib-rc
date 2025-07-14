#include <pthread.h>
#include "pthread_internal.h"
#include <errno.h>

int pthread_cancel(pthread_t thread)
{
    long result = sys_p_thread_ctrl(THREAD_CTRL_CANCEL, 
                                   (long)PTHREAD_CANCELED, 
                                   thread);
    return (result < 0) ? -result : 0;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    long result = sys_p_thread_ctrl(THREAD_CTRL_SETCANCELSTATE, 
                                   state, 
                                   (long)oldstate);
    return (result < 0) ? -result : 0;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    long result = sys_p_thread_ctrl(THREAD_CTRL_SETCANCELTYPE, 
                                   type, 
                                   (long)oldtype);
    return (result < 0) ? -result : 0;
}

void pthread_testcancel(void)
{
    sys_p_thread_ctrl(THREAD_CTRL_TESTCANCEL, 0, 0);
}