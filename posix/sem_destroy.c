#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <mint/mintbind.h>
#include "pthread_internal.h"

__typeof__(sem_destroy) __sem_destroy;

/**
 * @brief Destroys a named semaphore.
 *
 * @param sem Pointer to the semaphore to be destroyed.
 *
 * @return 0 on success, -1 on error.
 *
 * @details Named semaphores are only supported in single-threaded mode.
 *          Returns ENOSYS if called in multithreaded mode.
 *          Returns EINVAL if sem is NULL or sem->sem_id is NULL.
 *          Returns EBUSY if the semaphore is currently in use.
 *          Frees the memory allocated for sem->sem_id.
 */
int __sem_destroy(sem_t *sem)
{
    int32_t sem_id;
    int ret;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    /* Multithreaded mode: use pthread semaphore implementation */
    if (sem->wait_queue) {
        errno = EBUSY;
        return -1;
    }
    
    sem->count = 0;
    sem->wait_queue = NULL;
    
    /* Single-threaded mode: use original implementation */
    if (!sem->sem_id) {
        errno = EINVAL;
        return -1;
    }

    sem_id = ((int32_t)sem->sem_id[0] << 24) | 
            ((int32_t)sem->sem_id[1] << 16) | 
            ((int32_t)sem->sem_id[2] << 8) | 
            (int32_t)sem->sem_id[3];

    ret = Psemaphore(1, sem_id, 0);
    
    Mfree(sem->sem_id);
    sem->sem_id = NULL;
    
    return ret; /* Should be E_OK */
}

weak_alias (__sem_destroy, sem_destroy)