#include <semaphore.h>
#include <time.h>

int sem_clockwait(sem_t *sem, clockid_t clock_id, const struct timespec *abs_timeout)
{
    return sem_waitcommon(sem, 1, abs_timeout, clock_id);
}