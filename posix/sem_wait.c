#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include "pthread_internal.h"

__typeof__(sem_wait) __sem_wait;

/**
 * @brief Waits for a semaphore.
 *
 * @param sem Pointer to the semaphore structure.
 *
 * @return 0 on success, -1 on failure with `errno` set to indicate the error.
 *
 * @details In multithreaded mode, uses the pthread semaphore implementation.
 *          In single-threaded mode, uses the original implementation.
 *          Returns EINVAL if `sem` is NULL or `sem->sem_id` is NULL.
 */
int __sem_wait(sem_t *sem)
{
    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (_sem_is_multithreaded()) {
        /* Multithreaded mode: use pthread semaphore implementation */
        long result = sys_p_thread_sync(THREAD_SYNC_SEM_WAIT, (long)sem, 0);
        return (result < 0) ? -1 : 0;
    } else {
        /* Single-threaded mode: use original implementation */
        if (!sem->sem_id) {
            errno = EINVAL;
            return -1;
        }
        return sem_waitcommon(sem, 1, NULL, 0);
    }
}

 weak_alias (__sem_wait, sem_wait)