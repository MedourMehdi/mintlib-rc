#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <mint/mintbind.h>
#include "pthread_internal.h"

__typeof__(sem_post) __sem_post;

/**
 * @brief Post a semaphore.
 *
 * @param sem Pointer to the semaphore structure.
 *
 * @return 0 on success, -1 on failure with `errno` set to indicate the error.
 *
 * @details In multithreaded mode, uses the pthread semaphore implementation.
 *          In single-threaded mode, uses the original implementation.
 *          Returns EINVAL if `sem` is NULL or `sem->sem_id` is NULL.
 *          Returns EOVERFLOW if `sem->max_count` is equal to SEM_VALUE_MAX.
 */
int __sem_post(sem_t *sem)
{
    int32_t sem_id;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (_sem_is_multithreaded()) {
        /* Multithreaded mode: use pthread semaphore implementation */
        long result = sys_p_thread_sync(THREAD_SYNC_SEM_POST, (long)sem, 0);
        return (result < 0) ? -1 : 0;
    } else {
        /* Single-threaded mode: use original implementation */
        if (!sem->sem_id) {
            errno = EINVAL;
            return -1;
        }

        sem_id = ((int32_t)sem->sem_id[0] << 24) | 
                ((int32_t)sem->sem_id[1] << 16) | 
                ((int32_t)sem->sem_id[2] << 8) | 
                (int32_t)sem->sem_id[3];

        if (sem->max_count >= SEM_VALUE_MAX) {
            errno = EOVERFLOW;
            return -1;
        }

        sem->max_count++;
        
        if (Psemaphore(3, sem_id, 0) < 0) {
            sem->max_count--; /* Rollback on error */
            return -1;
        }
    }
    return 0;
}

weak_alias (__sem_post, sem_post)