#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include "pthread_internal.h"

__typeof__(sem_getvalue) __sem_getvalue;

/**
 * @brief Retrieve the current value of a semaphore.
 *
 * @param sem Pointer to the semaphore structure.
 * @param sval Pointer to an integer where the semaphore's value will be stored.
 * @return 0 on success, -1 on error with `errno` set to indicate the error.
 *
 * @details In multithreaded mode, retrieves the semaphore value using
 *          the pthread semaphore implementation. In single-threaded mode,
 *          retrieves the semaphore value using the original implementation.
 *          Returns EINVAL if `sem` or `sval` is NULL, or if in single-threaded
 *          mode and `sem->sem_id` is NULL.
 */

int __sem_getvalue(sem_t *sem, int *sval)
{
    if (!sem || !sval) {
        errno = EINVAL;
        return -1;
    }

    if (_sem_is_multithreaded()) {
        /* Multithreaded mode: use pthread semaphore implementation */
        *sval = sem->count;
    } else {
        /* Single-threaded mode: use original implementation */
        if (!sem->sem_id) {
            errno = EINVAL;
            return -1;
        }
        *sval = sem->max_count;
    }
    return 0;
}

weak_alias (__sem_getvalue, sem_getvalue)