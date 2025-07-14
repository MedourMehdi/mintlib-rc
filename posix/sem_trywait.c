#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include "pthread_internal.h"

__typeof__(sem_trywait) __sem_trywait;

/**
 * @brief Try to decrement the semaphore value and return immediately.
 *
 * @param sem Pointer to the semaphore structure.
 *
 * @return 0 on success, -1 on failure with `errno` set to indicate the error.
 *
 * @details In multithreaded mode, this function uses the pthread semaphore
 *          implementation. It checks if the semaphore value is greater than zero
 *          atomically and decrements it if so. If not, it returns immediately
 *          with EAGAIN.
 *
 *          In single-threaded mode, the function delegates the wait operation
 *          to `sem_waitcommon` with a timeout of zero.
 *          Returns EINVAL if `sem` or `sem->sem_id` is NULL.
 */
int __sem_trywait(sem_t *sem)
{
    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (_sem_is_multithreaded()) {
        /* Multithreaded mode: use pthread semaphore implementation */
        /* Check if semaphore has available count atomically */
        unsigned short sr = 0;
        asm volatile ("move.w %%sr,%0" : "=d" (sr));
#ifdef __mcoldfire__
        asm volatile ("move.w #0x2700,%%sr" : : : "memory");
#else
        asm volatile ("ori.w #0x0700,%%sr" : : : "memory");
#endif
        
        if (sem->count > 0) {
            sem->count--;
            asm volatile ("move.w %0,%%sr" : : "d" (sr) : "memory");
            return 0;
        }
        
        asm volatile ("move.w %0,%%sr" : : "d" (sr) : "memory");
        errno = EAGAIN;
        return -1;
    } else {
        /* Single-threaded mode: use original implementation */
        if (!sem->sem_id) {
            errno = EINVAL;
            return -1;
        }
        return sem_waitcommon(sem, 0, NULL, 0);
    }
}

weak_alias (__sem_trywait, sem_trywait)