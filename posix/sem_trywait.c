#include <semaphore.h>

int sem_trywait(sem_t *sem)
{
    return sem_waitcommon(sem, 0, NULL, 0);
}