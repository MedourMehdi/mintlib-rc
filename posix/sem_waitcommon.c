#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <mint/mintbind.h>

int sem_waitcommon(sem_t *sem, int blocking, const struct timespec *abs_timeout, clockid_t clock_id)
{
    int32_t sem_id;
    struct timespec now;
    int timeout_ms = -1;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (!sem->sem_id) {
        errno = EINVAL;
        return -1;
    }

    sem_id = ((int32_t)sem->sem_id[0] << 24) | 
             ((int32_t)sem->sem_id[1] << 16) | 
             ((int32_t)sem->sem_id[2] << 8) | 
             (int32_t)sem->sem_id[3];

    /* Handle timeout calculation if abs_timeout is provided */
    if (abs_timeout) {
        if (clock_gettime(clock_id, &now) != 0) {
            errno = EINVAL;
            return -1;
        }

        /* Check if timeout has already passed */
        if (now.tv_sec > abs_timeout->tv_sec || 
            (now.tv_sec == abs_timeout->tv_sec && now.tv_nsec >= abs_timeout->tv_nsec)) {
            errno = ETIMEDOUT;
            return -1;
        }

        /* Calculate timeout in milliseconds */
        timeout_ms = (abs_timeout->tv_sec - now.tv_sec) * 1000 + 
                     (abs_timeout->tv_nsec - now.tv_nsec) / 1000000;
    }

    /* Non-blocking case */
    if (!blocking) {
        if (sem->max_count <= 0) {
            errno = EAGAIN;
            return -1;
        }
        if (Psemaphore(2, sem_id, 0) < 0) {
            return -1;
        }
        sem->max_count--;
        return 0;
    }

    /* Blocking case */
    if (abs_timeout) {
        /* Timed wait - simplified implementation */
        while (sem->max_count <= 0) {
            if (clock_gettime(clock_id, &now) != 0) {
                errno = EINVAL;
                return -1;
            }
            
            if (now.tv_sec > abs_timeout->tv_sec || 
                (now.tv_sec == abs_timeout->tv_sec && now.tv_nsec >= abs_timeout->tv_nsec)) {
                errno = ETIMEDOUT;
                return -1;
            }
            
            sched_yield();
        }
    } else {
        /* Infinite wait */
        while (sem->max_count <= 0) {
            sched_yield();
        }
    }

    sem->max_count--;
    if (Psemaphore(2, sem_id, timeout_ms) < 0) {
        return -1;
    }

    return 0;
}