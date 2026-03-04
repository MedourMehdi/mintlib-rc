#include <pthread.h>
#include "pthread_priv.h"
#include <errno.h>

/* =========================== */
/*   pthread_setschedparam     */
/* =========================== */

__typeof__(pthread_setschedparam) __pthread_setschedparam;

int __pthread_setschedparam(pthread_t thread, int policy, 
                             const struct sched_param *param)
{
    long result;
    
    if (!param) return EINVAL;
    
    result = sys_p_thread_sched_policy(PSCHED_SETPARAM, thread, policy, param->sched_priority);
    return (result < 0) ? -result : 0;
}
weak_alias (__pthread_setschedparam, pthread_setschedparam)


/* =========================== */
/*   pthread_getschedparam     */
/* =========================== */

__typeof__(pthread_getschedparam) __pthread_getschedparam;

int __pthread_getschedparam(pthread_t thread, int *policy,
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
weak_alias (__pthread_getschedparam, pthread_getschedparam)


/* =========================== */
/*   pthread_setschedprio      */
/* =========================== */

__typeof__(pthread_setschedprio) __pthread_setschedprio;

int __pthread_setschedprio(pthread_t thread, int prio)
{
    struct sched_param param;
    int policy;
    
    /* 
     * Use internal __ versions to ensure we call the library's 
     * implementation directly, avoiding recursion or user overrides.
     */
    int result = __pthread_getschedparam(thread, &policy, &param);
    if (result != 0) return result;
    
    param.sched_priority = prio;
    return __pthread_setschedparam(thread, policy, &param);
}
weak_alias (__pthread_setschedprio, pthread_setschedprio)


/* =========================== */
/*  pthread_getconcurrency     */
/* =========================== */

__typeof__(pthread_getconcurrency) __pthread_getconcurrency;

int __pthread_getconcurrency(void)
{
    return 1;  // FreeMiNT is single-processor
}
weak_alias (__pthread_getconcurrency, pthread_getconcurrency)


/* =========================== */
/*  pthread_setconcurrency     */
/* =========================== */

__typeof__(pthread_setconcurrency) __pthread_setconcurrency;

int __pthread_setconcurrency(int level)
{
    return 0;  // Ignored on single-processor systems
}
weak_alias (__pthread_setconcurrency, pthread_setconcurrency)