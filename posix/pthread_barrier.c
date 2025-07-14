#include <pthread.h>
#include "pthread_internal.h"
#include <errno.h>

int pthread_barrier_init(pthread_barrier_t *barrier, 
                        const pthread_barrierattr_t *attr,
                        unsigned int count)
{
    int result;

    (void)attr; // Attributes not currently used
    if (!barrier || count == 0) return EINVAL;
    
    result = pthread_mutex_init(&barrier->mutex, NULL);
    if (result != 0) return result;
    
    result = pthread_cond_init(&barrier->cond, NULL);
    if (result != 0) {
        pthread_mutex_destroy(&barrier->mutex);
        return result;
    }
    
    barrier->count = count;
    barrier->waiting = 0;
    barrier->generation = 0;
    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    int result;

    if (!barrier) return EINVAL;
    
    result = pthread_mutex_lock(&barrier->mutex);
    if (result != 0) return result;
    
    if (barrier->waiting > 0) {
        pthread_mutex_unlock(&barrier->mutex);
        return EBUSY;
    }
    
    pthread_mutex_unlock(&barrier->mutex);
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    int result;
    unsigned int gen;

    if (!barrier) return EINVAL;
    
    result = pthread_mutex_lock(&barrier->mutex);
    if (result != 0) return result;
    
    gen = barrier->generation;
    barrier->waiting++;
    
    if (barrier->waiting == barrier->count) {
        barrier->waiting = 0;
        barrier->generation++;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return PTHREAD_BARRIER_SERIAL_THREAD;
    }
    
    while (gen == barrier->generation) {
        result = pthread_cond_wait(&barrier->cond, &barrier->mutex);
        if (result != 0) {
            barrier->waiting--;
            pthread_mutex_unlock(&barrier->mutex);
            return result;
        }
    }
    
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
}