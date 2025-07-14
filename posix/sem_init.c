#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <mint/mintbind.h>
#include "pthread_internal.h"

static char *gen_sem_id(int16_t length) {
    int n;
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        
    char *randomString = NULL;

    if (length) {
        randomString = (char*)Mxalloc(sizeof(char) * (length + 1), 3);

        if (randomString) {            
            for (n = 0; n < length; n++) {            
                int key = rand() % (int)(sizeof(charset) - 1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

int _sem_is_multithreaded(void)
{
    return pthread_is_multithreaded_np();
}

__typeof__(sem_init) __sem_init;

/**
 * @brief Initialize a semaphore.
 *
 * @param sem Pointer to the semaphore structure.
 * @param pshared Flag indicating if the semaphore is shared between processes.
 * @param value Initial value of the semaphore.
 *
 * @return 0 on success, -1 on error with `errno` set to indicate the error.
 *
 * @details In multithreaded mode, initializes the semaphore using the
 *          pthread semaphore implementation. In single-threaded mode,
 *          initializes the semaphore using the original implementation.
 *          Returns EINVAL if `sem` is NULL, or if `value` is greater than
 *          SEM_VALUE_MAX, or if `pshared` is not 0.
 *          Returns ENOSYS if `pshared` is not 0.
 *          Returns ENOMEM if memory for the semaphore structure could not be
 *          allocated.
 */
int __sem_init(sem_t *sem, int pshared, unsigned int value)
{
    int32_t sem_id;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (value > SEM_VALUE_MAX) {
        errno = EINVAL;
        return -1;
    }

    if (pshared > 0) {
        errno = ENOSYS;
        return -1;
    }

    /* Multithreaded mode: use pthread semaphore implementation */
    sem->count = value;
    sem->wait_queue = NULL;
    // /* Use MiNT thread operation system call for semaphore init */
    // result = sys_p_thread_sync(THREAD_SYNC_SEM_INIT, (long)sem, 0);
    // return (result < 0) ? -1 : 0;
    /* Single-threaded mode: use original implementation */
    sem->max_count = value;
    sem->io_count = 0;
    
    sem->sem_id = gen_sem_id(4);
    if (!sem->sem_id) {
        errno = ENOMEM;
        return -1;
    }

    sem_id = ((int32_t)sem->sem_id[0] << 24) | 
                ((int32_t)sem->sem_id[1] << 16) | 
                ((int32_t)sem->sem_id[2] << 8) | 
                (int32_t)sem->sem_id[3];

    if (Psemaphore(0, sem_id, 0) < 0) {
        Mfree(sem->sem_id);
        return -1;
    }

    return 0;
}

weak_alias (__sem_init, sem_init)