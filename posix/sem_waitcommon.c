#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <mint/mintbind.h>
#include "pthread_internal.h"

__typeof__(sem_waitcommon) __sem_waitcommon;

/**
 * @brief Internal helper function for sem_clockwait, sem_wait, sem_timedwait, sem_trywait.
 *
 * @param sem Pointer to the semaphore structure.
 * @param blocking Whether to block (1) or not (0).
 * @param abs_timeout Absolute timeout as a `timespec` structure, or NULL for infinite wait.
 * @param clock_id The clock against which the timeout is measured.
 * @return 0 on success, -1 on failure with `errno` set to indicate the error.
 *
 * @details This function is only used in single-threaded mode.
 *          Returns ENOSYS if called in multithreaded mode.
 *          Returns EINVAL if sem is NULL or sem->sem_id is NULL.
 *          Returns EINVAL if the semaphore could not be accessed.
 *          Returns EAGAIN if the semaphore value is zero and blocking is 0.
 *          Returns ETIMEDOUT if the semaphore value is zero and the specified timeout has passed.
 */
int __sem_waitcommon(sem_t *sem, int blocking, const struct timespec *abs_timeout, clockid_t clock_id)
{
    int32_t sem_id;
    struct timespec now;
    int timeout_ms = -1;

    /* This function is only used in single-threaded mode */
    if (_sem_is_multithreaded()) {
        errno = ENOSYS;
        return -1;
    }

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

weak_alias (__sem_waitcommon, sem_waitcommon)