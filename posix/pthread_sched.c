
#include <pthread.h>
#include "pthread_internal.h"
#include <errno.h>

int pthread_setschedparam(pthread_t thread, int policy, 
                         const struct sched_param *param)
{
    long result;
    
    if (!param) return EINVAL;
    
    result = sys_p_thread_sched_policy(PSCHED_SETPARAM, thread, policy, param->sched_priority);
    return (result < 0) ? -result : 0;
}

int pthread_getschedparam(pthread_t thread, int *policy,
                         struct sched_param *param)
{
    long p, pri, result;
    if (!policy || !param) return EINVAL;
    
    
    result = sys_p_thread_sched_policy(PSCHED_GETPARAM, thread, (long)&p, (long)&pri);
    
    if (result < 0) return -result;
    
    *policy = p;
    param->sched_priority = pri;
    return 0;
}

int pthread_setschedprio(pthread_t thread, int prio)
{
    struct sched_param param;
    int policy;
    
    int result = pthread_getschedparam(thread, &policy, &param);
    if (result != 0) return result;
    
    param.sched_priority = prio;
    return pthread_setschedparam(thread, policy, &param);
}

int pthread_getconcurrency(void)
{
    return 1;  // FreeMiNT is single-processor
}

int pthread_setconcurrency(int level)
{
    return 0;  // Ignored on single-processor systems
}