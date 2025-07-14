#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include <mint/mintbind.h>
#include <mint/dcntl.h>
#include "pthread_internal.h"

__typeof__(sem_open) __sem_open;

/**
 * @brief Opens or creates a named semaphore.
 *
 * @param name The name of the semaphore. It must start with a '/' and 
 *        not exceed SEM_NAME_MAX characters. The name cannot be "/" or start with '_'.
 * @param oflag Flags indicating the action: O_CREAT to create a new semaphore,
 *        optionally combined with O_EXCL to ensure the semaphore does not
 *        already exist.
 * @param ... Additional arguments when O_CREAT is specified: mode_t mode for
 *        permissions and unsigned int value for initial semaphore value.
 *
 * @return A pointer to the semaphore on success, SEM_FAILED on error.
 *
 * @details Named semaphores are only supported in single-threaded mode.
 *          Returns ENOSYS if called in multithreaded mode.
 *          Returns EINVAL if name is NULL, invalid, or too long.
 *          Returns ENAMETOOLONG if name exceeds SEM_NAME_MAX.
 *          Returns EEXIST if O_EXCL is set and semaphore exists.
 *          Returns ENOMEM if memory allocation fails.
 *          Returns EACCES if access permissions are insufficient.
 *          Returns ENOENT if the semaphore does not exist.
 *          Returns EMFILE if the maximum number of file handles is reached.
 */

sem_t *__sem_open(const char* name, int oflag, ...)
{
    char sem_path[12] = {'\0'};
    sem_t* sem_ptr = NULL;
    int handle;
    int32_t sem_id;
    mode_t mode = 0;
    unsigned int value = 0;
    va_list args;

    /* Named semaphores are only supported in single-threaded mode */
    if (_sem_is_multithreaded()) {
        errno = ENOSYS;
        return SEM_FAILED;
    }

    if (!name) {
        errno = EINVAL;
        return SEM_FAILED;
    }

    if (strlen(name) > SEM_NAME_MAX) {
        errno = ENAMETOOLONG;
        return SEM_FAILED;
    }

    if (name[0] != '/') {
        errno = EINVAL;
        return SEM_FAILED;
    }

    if (strcmp("/", name) == 0) {
        errno = EINVAL;
        return SEM_FAILED;
    }

    if (name[0] == '_') {
        errno = ENOENT;
        return SEM_FAILED;
    }

    /* Extract additional arguments if creating */
    if (oflag & O_CREAT) {
        va_start(args, oflag);
        mode = va_arg(args, mode_t);
        value = va_arg(args, unsigned int);
        va_end(args);

        if (value > SEM_VALUE_MAX) {
            errno = EINVAL;
            return SEM_FAILED;
        }
    }

    strcpy(&sem_path[0], "/U/SHM");
    strcpy(&sem_path[6], name);

    if (oflag & O_CREAT) {
        if (oflag & O_EXCL) {
            if (access(sem_path, F_OK) == 0) {
                errno = EEXIST;
                return SEM_FAILED;
            }
        }

        handle = Fcreate((const char*)sem_path, 0x0004);
 
        if (handle == ENHNDL) {
            errno = EMFILE;
            return SEM_FAILED;
        }

        if (handle == EPTHNF || handle == EACCDN) {
            errno = EACCES;
            return SEM_FAILED;
        }

        if (handle < 0) {
            return SEM_FAILED;
        }

        Fchmod(sem_path, mode);
        
        sem_ptr = (sem_t*)Mxalloc(sizeof(sem_t), 3);
        
        if (sem_ptr == NULL) {
            Fclose(handle);
            errno = ENOMEM;
            return SEM_FAILED;
        }

        /* Initialize unified structure fields */
        sem_ptr->count = 0;
        sem_ptr->wait_queue = NULL;

        sem_ptr->sem_id = (char*)Mxalloc(SEM_NAME_MAX + 1, 3);
        if (!sem_ptr->sem_id) {
            Mfree(sem_ptr);
            Fclose(handle);
            errno = ENOMEM;
            return SEM_FAILED;
        }

        Fcntl(handle, sem_ptr, SHMSETBLK);

        strncpy(sem_ptr->sem_id, name, SEM_NAME_MAX);
        sem_ptr->sem_id[SEM_NAME_MAX] = '\0';

        sem_ptr->max_count = value;
        sem_ptr->io_count = 0;

        sem_id = ((int32_t)sem_ptr->sem_id[0] << 24) | 
                 ((int32_t)sem_ptr->sem_id[1] << 16) | 
                 ((int32_t)sem_ptr->sem_id[2] << 8) | 
                 (int32_t)sem_ptr->sem_id[3];

        if (Psemaphore(0, sem_id, 0) < 0) {
            Mfree(sem_ptr->sem_id);
            Mfree(sem_ptr);
            Fclose(handle);
            return SEM_FAILED;
        }

        Fclose(handle);
    } else {
        if (access(sem_path, F_OK) != 0) {
            errno = ENOENT;
            return SEM_FAILED;
        }
        if (access(sem_path, R_OK) != 0) {
            errno = EACCES;
            return SEM_FAILED;
        }

        /* Initialize unified structure fields for existing semaphore */
        sem_ptr->count = 0;
        sem_ptr->wait_queue = NULL;

        handle = Fopen(sem_path, 0x0001);
        if (handle < 0) {
            errno = EACCES;
            return SEM_FAILED;
        }

        sem_ptr = (sem_t*)Fcntl(handle, 0, SHMGETBLK);
        if (!sem_ptr) {
            Fclose(handle);
            errno = ENOENT;
            return SEM_FAILED;
        }

        Fclose(handle);
    }

    sem_ptr->io_count++;

    return sem_ptr;
}

weak_alias (__sem_open, sem_open)