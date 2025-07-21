#include <errno.h>
#include <pthread.h>
#include "pthread_priv.h"

int pthread_attr_init(pthread_attr_t *attr)
{
    if (!attr) return EINVAL;
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
    attr->stacksize = 0;
    attr->policy = SCHED_FIFO;
    attr->priority = 0;
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    if (!attr) return EINVAL;
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    if (!attr || (detachstate != PTHREAD_CREATE_JOINABLE && 
                 detachstate != PTHREAD_CREATE_DETACHED))
        return EINVAL;
    attr->detachstate = detachstate;
    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
    if (!attr || !detachstate) return EINVAL;
    *detachstate = attr->detachstate;
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if (!attr || stacksize < 16384) return EINVAL;
    attr->stacksize = stacksize;
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
    if (!attr || !stacksize) return EINVAL;
    *stacksize = attr->stacksize;
    return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    if (!attr || (policy != SCHED_OTHER && 
                 policy != SCHED_FIFO && 
                 policy != SCHED_RR))
        return EINVAL;
    attr->policy = policy;
    return 0;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    if (!attr || !policy) return EINVAL;
    *policy = attr->policy;
    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if (!attr || !param) return EINVAL;
    attr->priority = param->sched_priority;
    return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
    if (!attr || !param) return EINVAL;
    param->sched_priority = attr->priority;
    return 0;
}