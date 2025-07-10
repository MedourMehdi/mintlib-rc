#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int sem_close(sem_t* sem)
{
    char sem_path[12] = {'\0'};
    sem_t* sem_ptr;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (!sem->sem_id) {
        errno = EINVAL;
        return -1;
    }

    strcpy(&sem_path[0], "/U/SHM");
    strcpy(&sem_path[6], sem->sem_id);

    if (access(sem_path, W_OK) != 0) {
        errno = EINVAL;
        return -1;
    }

    sem_ptr = sem_open(sem->sem_id, 0);

    if (sem_ptr == SEM_FAILED) {
        errno = EINVAL;
        return -1;
    }

    if (sem_ptr->io_count > 0) {
        sem_ptr->io_count--;
    }

    return 0;
}