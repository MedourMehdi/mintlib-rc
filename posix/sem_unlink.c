#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <mint/mintbind.h>

int sem_unlink(const char* name)
{
    char sem_path[12] = {'\0'};
    int32_t sem_id;
    int ret;
    sem_t* sem_ptr;
    pid_t this_fork;

    if (!name) {
        errno = EINVAL;
        return -1;
    }

    if (strlen(name) > SEM_NAME_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    } 

    strcpy(&sem_path[0], "/U/SHM");
    strcpy(&sem_path[6], name);

    if (access(sem_path, W_OK) != 0) {
        errno = EACCES;
        return -1;
    }

    sem_ptr = sem_open(name, 0);
    
    if (sem_ptr == SEM_FAILED) {
        errno = ENOENT;
        return -1;
    }

    sem_ptr->io_count--;

    /* Fork a process to handle the actual unlinking */
    this_fork = fork();
    if (this_fork == 0) {
        /* Child process */
        while (sem_ptr->io_count > 0) {
            sched_yield();
        }
        
        sem_id = ((int32_t)sem_ptr->sem_id[0] << 24) | 
                 ((int32_t)sem_ptr->sem_id[1] << 16) | 
                 ((int32_t)sem_ptr->sem_id[2] << 8) | 
                 (int32_t)sem_ptr->sem_id[3];

        Psemaphore(1, &sem_id, 0);

        Mfree(sem_ptr->sem_id);
        Mfree(sem_ptr);

        ret = Fdelete(sem_path);

        if (ret == EFILNF) {
            errno = ENOENT;
            return -1;
        }

        if (ret == EACCDN) {
            errno = EACCES;
            return -1;
        }

        if (ret >= 0) {
            return 0;
        }
        
        return -1;
    } else if (this_fork < 0) {
        /* Fork failed */
        errno = EAGAIN;
        return -1;
    }

    /* Parent process returns immediately */
    return 0;
}