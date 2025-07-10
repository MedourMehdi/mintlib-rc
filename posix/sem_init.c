#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <mint/mintbind.h>

char *gen_sem_id(int16_t length) {
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

int sem_init(sem_t *sem, int pshared, unsigned int value)
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

    sem->sem_id = gen_sem_id(4);
    if (!sem->sem_id) {
        errno = ENOMEM;
        return -1;
    }

    sem_id = ((int32_t)sem->sem_id[0] << 24) | 
             ((int32_t)sem->sem_id[1] << 16) | 
             ((int32_t)sem->sem_id[2] << 8) | 
             (int32_t)sem->sem_id[3];

    sem->max_count = value;
    sem->io_count = 0;

    if (Psemaphore(0, sem_id, 0) < 0) {
        Mfree(sem->sem_id);
        return -1;
    }

    return 0;
}