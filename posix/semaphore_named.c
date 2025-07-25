
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <mint/mintbind.h>
#include <mint/dcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "semaphore.h"
#include "semaphore_priv.h"
#include <stdarg.h>

sem_t *sem_open(const char *name, int oflag, ...) {
    char sem_path[16] = "/SHM";
    sem_t* sem_ptr = NULL;
    int handle, result;
    int32_t sem_id;

    mode_t mode = 0;
    unsigned int value = 0;

    va_list args;

    if (!name || name[0] != '/' || strcmp(name, "/") == 0) {
        errno = EINVAL;
        return SEM_FAILED;
    }

    if (strlen(name) > SEM_NAME_MAX + 1 || name[1] == '_') {
        errno = (name[1] == '_') ? ENOENT : ENAMETOOLONG;
        return SEM_FAILED;
    }

    if (__mint_is_multithreaded) {
        errno = ENOSYS;
        return SEM_FAILED;
    }

    strncpy(sem_path + strlen(sem_path), name, sizeof(sem_path) - 7);
    sem_path[sizeof(sem_path) - 1] = '\0';

    if (oflag & O_CREAT) {
        va_start(args, oflag);
        mode = va_arg(args, mode_t);
        value = va_arg(args, unsigned int);
        va_end(args);

        if (value > SEM_VALUE_MAX) {
            errno = EINVAL;
            return SEM_FAILED;
        }

        if ((oflag & O_EXCL) && access(sem_path, F_OK) == 0) {
            errno = EEXIST;
            return SEM_FAILED;
        }

        handle = Fcreate(sem_path, 0x0004);
        if (handle < 0) {
            errno = (handle == ENHNDL) ? EMFILE : ENOENT;
            return SEM_FAILED;
        }

        Fchmod(sem_path, mode);
        Fclose(handle);  // Close after creating, we'll reopen if needed
        sem_ptr = (sem_t*)Mxalloc(sizeof(sem_t), 3);

        if (!sem_ptr) {
            errno = ENOMEM;
            return SEM_FAILED;
        }

        // Reopen for shared memory attachment
        handle = Fopen(sem_path, 0x0001);
        if (handle < 0) {
            Mfree(sem_ptr);
            return SEM_FAILED;
        }

        Fcntl(handle, (long)sem_ptr, SHMSETBLK);

        /* Store name without leading '/' */
        strncpy(sem_ptr->sem_id, name + 1, SEM_NAME_MAX);
        sem_ptr->sem_id[SEM_NAME_MAX] = '\0';
        sem_ptr->count = value;
        sem_ptr->io_count = 0;

        sem_id = sem_id_from_name(sem_ptr->sem_id);
        result = Psemaphore(0, sem_id, 0);
        if (result < 0) {
            Mfree(sem_ptr);
            Fclose(handle);
            return SEM_FAILED;
        }

        /* Always release after creation - we own it immediately */
        Psemaphore(3, sem_id, 0);
    } else {
        if (access(sem_path, R_OK) != 0) {
            errno = ENOENT;
            return SEM_FAILED;
        }

        handle = Fopen(sem_path, 0x0001);
        if (handle < 0) {
            errno = ENOENT;
            return SEM_FAILED;
        }

        sem_ptr = (sem_t*)Fcntl(handle, 0, SHMGETBLK);
        Fclose(handle);

        if (!sem_ptr) {
            errno = ENOENT;
            return SEM_FAILED;
        }
    }

    sem_ptr->io_count++;
    return sem_ptr;
}

int sem_unlink(const char* name) {
    char sem_path[16] = "/SHM";
    sem_t* sem_ptr;

    if (__mint_is_multithreaded) {
        errno = ENOSYS;
        return -1;
    }

    if (strlen(name) > SEM_NAME_MAX + 1) {
        errno = ENAMETOOLONG;
        return -1;
    }

    strncat(sem_path, name, sizeof(sem_path) - strlen(sem_path) - 1);
    sem_path[sizeof(sem_path) - 1] = '\0';

    sem_ptr = sem_open(name, 0);
    if (sem_ptr == SEM_FAILED) {
        errno = ENOENT;
        return -1;
    }

    sem_ptr->io_count--;  // one less reference

    if (sem_ptr->io_count <= 0) {
        int32_t sem_id = sem_id_from_name(sem_ptr->sem_id);
        Psemaphore(1, sem_id, 0);
        Fdelete(sem_path);
    }

    return 0;
}

int sem_close(sem_t* sem) {
    int32_t sem_id;
    char sem_path[16] = "/SHM/";

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    sem->io_count--;

    if (sem->io_count == 0) {
        sem_id = sem_id_from_name(sem->sem_id);
        
        // Build semaphore path
        strncat(sem_path, sem->sem_id, sizeof(sem_path) - strlen(sem_path) - 1);
        sem_path[sizeof(sem_path) - 1] = '\0';

        // Destroy system resources
        Psemaphore(1, sem_id, 0);
        Fdelete(sem_path);
        Mfree(sem);
    }

    return 0;
}