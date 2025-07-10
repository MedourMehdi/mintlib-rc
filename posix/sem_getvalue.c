#include <semaphore.h>
#include <errno.h>

int sem_getvalue(sem_t *sem, int *sval)
{
    if (!sem || !sval) {
        errno = EINVAL;
        return -1;
    }

    if (!sem->sem_id) {
        errno = EINVAL;
        return -1;
    }

    *sval = sem->max_count;
    return 0;
}