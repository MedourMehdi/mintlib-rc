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

        unsigned short old_count = sem->count;

        /* Multithreaded mode: use atomic syscall for check-and-decrement */
        /* Try to atomically decrement count if it's > 0 */
        
        /* First check if count > 0 without decrementing */
        if (sem->count <= 0) {
            errno = EAGAIN;
            return -1;
        }
        
        /* Use atomic compare-and-swap to decrement if still > 0 */
        /* This will atomically check if count > 0 and decrement if so */
        
        while (old_count > 0) {
            /* Try to CAS from old_count to old_count-1 */
            if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&sem->count, (long)old_count, (long)(old_count - 1)) == 1) {
                /* Successfully decremented */
                return 0;
            }
            /* CAS failed, re-read current value and try again */
            old_count = sem->count;
            /* Yield to other threads to avoid busy-waiting */
            sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
        }
        
        /* Count is 0, cannot decrement */
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