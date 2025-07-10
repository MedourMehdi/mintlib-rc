#include <semaphore.h>
#include <time.h>

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    return sem_waitcommon(sem, 1, abs_timeout, CLOCK_REALTIME);
}