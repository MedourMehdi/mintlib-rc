#include <semaphore.h>
#include <errno.h>
#include <mint/mintbind.h>

int sem_post(sem_t *sem)
{
    int32_t sem_id;

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

    if (sem->max_count >= SEM_VALUE_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    sem->max_count++;
    
    if (Psemaphore(3, sem_id, 0) < 0) {
        sem->max_count--; /* Rollback on error */
        return -1;
    }

    return 0;
}