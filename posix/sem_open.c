#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <mint/mintbind.h>
#include <mint/dcntl.h>

sem_t *sem_open(const char* name, int oflag, ...)
{
    char sem_path[12] = {'\0'};
    sem_t* sem_ptr = NULL;
    int handle;
    int32_t sem_id;
    mode_t mode = 0;
    unsigned int value = 0;
    va_list args;

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