#include <semaphore.h>

int sem_wait(sem_t *sem)
{
    return sem_waitcommon(sem, 1, NULL, 0);
}