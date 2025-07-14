#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "pthread_internal.h"

__typeof__(sem_close) __sem_close;

/**
 * @brief Close a named semaphore.
 *
 * @param sem Pointer to the semaphore structure.
 *
 * @return 0 on success, -1 on error.
 *
 * @details Named semaphores are only supported in single-threaded mode.
 *          Returns ENOSYS if called in multithreaded mode.
 *          Returns EINVAL if sem is NULL or sem->sem_id is NULL.
 *          Returns EINVAL if sem->sem_id is not a valid named semaphore.
 *          Returns EINVAL if sem->sem_id is not open for writing.
 *          Returns EINVAL if the named semaphore could not be opened.
 *          Decrements the io_count of the semaphore.
 */
int __sem_close(sem_t* sem)
{
    char sem_path[12] = {'\0'};
    sem_t* sem_ptr;

    /* Named semaphores are only supported in single-threaded mode */
    if (_sem_is_multithreaded()) {
        errno = ENOSYS;
        return -1;
    }

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

weak_alias (__sem_close, sem_close)