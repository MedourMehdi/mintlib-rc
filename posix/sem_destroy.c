#include <semaphore.h>
#include <errno.h>
#include <mint/mintbind.h>

int sem_destroy(sem_t *sem)
{
    int32_t sem_id;
    int ret;

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

    ret = Psemaphore(1, &sem_id, 0);
    
    Mfree(sem->sem_id);
    sem->sem_id = NULL;
    
    return ret;
}