#include <errno.h>
#include <pthread.h>
#include <sched.h>  /* Required for SCHED_FIFO, SCHED_RR, etc. */
#include "pthread_priv.h"

/* =========================== */
/* pthread_attr_init/destroy  */
/* =========================== */

__typeof__(pthread_attr_init) __pthread_attr_init;

int __pthread_attr_init(pthread_attr_t *attr)
{
    if (!attr) return EINVAL;
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
    attr->stacksize = 0;
    attr->policy = SCHED_FIFO; /* Or SCHED_OTHER depending on default preference */
    attr->priority = 0;
    attr->inheritsched = PTHREAD_INHERIT_SCHED;
    return 0;
}
weak_alias (__pthread_attr_init, pthread_attr_init)


__typeof__(pthread_attr_destroy) __pthread_attr_destroy;

int __pthread_attr_destroy(pthread_attr_t *attr)
{
    /* Nothing to dynamically allocate in this simple struct implementation */
    if (!attr) return EINVAL;
    return 0;
}
weak_alias (__pthread_attr_destroy, pthread_attr_destroy)


/* =========================== */
/*      Detach State           */
/* =========================== */

__typeof__(pthread_attr_setdetachstate) __pthread_attr_setdetachstate;

int __pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if (!attr || (detachstate != PTHREAD_CREATE_JOINABLE && 
                 detachstate != PTHREAD_CREATE_DETACHED))
        return EINVAL;
    attr->detachstate = detachstate;
    return 0;
}
weak_alias (__pthread_attr_setdetachstate, pthread_attr_setdetachstate)


__typeof__(pthread_attr_getdetachstate) __pthread_attr_getdetachstate;

int __pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    if (!attr || !detachstate) return EINVAL;
    *detachstate = attr->detachstate;
    return 0;
}
weak_alias (__pthread_attr_getdetachstate, pthread_attr_getdetachstate)


/* =========================== */
/*        Stack Size           */
/* =========================== */

__typeof__(pthread_attr_setstacksize) __pthread_attr_setstacksize;

int __pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (!attr || stacksize < PTHREAD_STACK_MIN) return EINVAL;
    attr->stacksize = stacksize;
    return 0;
}
weak_alias (__pthread_attr_setstacksize, pthread_attr_setstacksize)


__typeof__(pthread_attr_getstacksize) __pthread_attr_getstacksize;

int __pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    if (!attr || !stacksize) return EINVAL;
    *stacksize = attr->stacksize;
    return 0;
}
weak_alias (__pthread_attr_getstacksize, pthread_attr_getstacksize)


/* =========================== */
/*       Scheduling Policy     */
/* =========================== */

__typeof__(pthread_attr_setschedpolicy) __pthread_attr_setschedpolicy;

int __pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    if (!attr || (policy != SCHED_OTHER && 
                 policy != SCHED_FIFO && 
                 policy != SCHED_RR))
        return EINVAL;
    attr->policy = policy;
    return 0;
}
weak_alias (__pthread_attr_setschedpolicy, pthread_attr_setschedpolicy)


__typeof__(pthread_attr_getschedpolicy) __pthread_attr_getschedpolicy;

int __pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    if (!attr || !policy) return EINVAL;
    *policy = attr->policy;
    return 0;
}
weak_alias (__pthread_attr_getschedpolicy, pthread_attr_getschedpolicy)


/* =========================== */
/*      Inherit Scheduling     */
/* =========================== */

__typeof__(pthread_attr_setinheritsched) __pthread_attr_setinheritsched;

int __pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) 
{
    if (!attr) return EINVAL;
    
    if (inheritsched != PTHREAD_INHERIT_SCHED && 
        inheritsched != PTHREAD_EXPLICIT_SCHED) {
        return EINVAL;
    }
    
    attr->inheritsched = inheritsched;
    return 0;
}
weak_alias (__pthread_attr_setinheritsched, pthread_attr_setinheritsched)


__typeof__(pthread_attr_getinheritsched) __pthread_attr_getinheritsched;

int __pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) 
{
    if (!attr || !inheritsched) return EINVAL;
    *inheritsched = attr->inheritsched;
    return 0;
}
weak_alias (__pthread_attr_getinheritsched, pthread_attr_getinheritsched)


/* =========================== */
/*       Scheduling Params     */
/* =========================== */

__typeof__(pthread_attr_setschedparam) __pthread_attr_setschedparam;

int __pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if (!attr || !param) return EINVAL;
    attr->priority = param->sched_priority;
    return 0;
}
weak_alias (__pthread_attr_setschedparam, pthread_attr_setschedparam)


__typeof__(pthread_attr_getschedparam) __pthread_attr_getschedparam;

int __pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
    if (!attr || !param) return EINVAL;
    param->sched_priority = attr->priority;
    return 0;
}
weak_alias (__pthread_attr_getschedparam, pthread_attr_getschedparam)