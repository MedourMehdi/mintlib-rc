#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <mint/mintbind.h>
#include <mint/dcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "semaphore.h"
#include "semaphore_priv.h"
#include "pthread_priv.h"
#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif
/* Generate a 4-character semaphore ID for FreeMiNT Psemaphore */
static char *gen_sem_id(void) {
    static int counter = 0;
    char *sem_id = (char*)Mxalloc(5, 3); /* 4 chars + null terminator */
    int pid;
    if (!sem_id) {
        return NULL;
    }
    
    /* Generate a unique 4-character ID using counter and process ID */
    /* Avoid names starting with underscore (reserved for OS) */
    pid = getpid();
    counter = (counter + 1) % 999; /* Keep counter manageable */
    
    /* Create 4-char ID: first char 'A'-'Z', rest alphanumeric */
    sem_id[0] = 'A' + (pid % 26);
    sem_id[1] = '0' + ((pid / 26) % 10);
    sem_id[2] = 'A' + (counter % 26);
    sem_id[3] = '0' + ((counter / 26) % 10);
    sem_id[4] = '\0';
    
    return sem_id;
}

/* Convert 4-character name to int32_t ID for Psemaphore */
int32_t name_to_sem_id(const char *name) {
    return ((int32_t)name[0] << 24) | 
           ((int32_t)name[1] << 16) | 
           ((int32_t)name[2] << 8) | 
           (int32_t)name[3];
}

int32_t sem_id_from_name(const char *name) {
    char sem_id_chars[5] = {'\0'};
    const char *id_source;
    size_t len;

    /* For named semaphores from sem_open, skip the '/' and process the rest */
    id_source = (name[0] == '/') ? name + 1 : name;
    len = strlen(id_source);
    
    if (len <= 4) {
        /* Short names: copy and pad with 'X' */
        size_t copy_len = (len > 4) ? 4 : len;
        memcpy(sem_id_chars, id_source, copy_len);
        /* Pad remaining positions with 'X' */
        for (size_t i = copy_len; i < 4; i++) {
            sem_id_chars[i] = 'X';
        }
    } else {
        /* Long names: use first char + hash of remaining chars */
        unsigned int hash = 0;
        sem_id_chars[0] = id_source[0];
        /* Simple hash function for remaining characters */
        for (size_t i = 1; i < len; i++) {
            hash = hash * 31 + (unsigned char)id_source[i];
        }
        
        /* Convert hash to 3 alphabetic characters */
        sem_id_chars[1] = 'A' + (hash % 26);
        sem_id_chars[2] = 'A' + ((hash / 26) % 26);
        sem_id_chars[3] = 'A' + ((hash / (26*26)) % 26);
    }
    
    /* Ensure no null chars in the middle - replace with 'X' */
    for (int i = 0; i < 4; i++) {
        if (sem_id_chars[i] == '\0') {
            sem_id_chars[i] = 'X';
        }
    }
    
    sem_id_chars[4] = '\0';
    
    return name_to_sem_id(sem_id_chars);
}

int sem_init(sem_t *sem, int pshared, unsigned int value) {
    int32_t sem_id;
    int result;

    if(value > SEM_VALUE_MAX) {
        errno = EINVAL;
        return -1;
    }

    if(pshared > 0) {
        errno = ENOSYS;
        return -1;
    }

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (pthread_is_multithreaded_np()) {
        /* Multithreaded mode: use pthread semaphore implementation */
        sem->count = value;
        sem->wait_queue = NULL;
        sem->io_count = 0;
        strcpy(sem->sem_id, "");  /* Not used in multithreaded mode */
    } else {
        /* Single-threaded mode: use original implementation */
        char *temp_id = gen_sem_id();

        if (!temp_id) {
            errno = ENOMEM;
            return -1;
        }
        
        strncpy(sem->sem_id, temp_id, SEM_NAME_MAX);
        sem->sem_id[SEM_NAME_MAX] = '\0';
        Mfree(temp_id);  /* Free the temporary allocated ID */

        sem_id = name_to_sem_id(sem->sem_id);
        sem->count = value;
        sem->io_count = 0;

        result = Psemaphore(0, sem_id, 0);
        if(result < 0) {
            strcpy(sem->sem_id, "");
            errno = (result == -1) ? EEXIST : EACCES;
            return -1;
        }

        /* Always release the semaphore after creation - we own it immediately */
        Psemaphore(3, sem_id, 0);  /* Release ownership */
    }

    return 0;
}

int sem_wait(sem_t *sem) {
    int result;
    /* Reset errno at the start of each call */
    errno = 0;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (pthread_is_multithreaded_np()) {

        /* Multithreaded mode: use pthread semaphore implementation */
        return (sys_p_thread_sync(THREAD_SYNC_SEM_WAIT, (long)sem, 0) < 0) ? -1 : 0;

    } else {

        /* Single-threaded mode: use original implementation */
        int32_t sem_id;

        if (sem->sem_id[0] == '\0') {
            errno = EINVAL;
            return -1;
        }

        sem_id = sem_id_from_name(sem->sem_id);

        /* Blocking wait loop for sem_wait */
        while (1) {
            /* Use Psemaphore as mutex to protect critical section */
            result = Psemaphore(2, sem_id, -1);  /* Lock - wait indefinitely */
            if (result < 0) {
                errno = EINVAL;
                return -1;
            }
            
            /* Critical section start */
            if (sem->count > 0) {
                sem->count--;
                Psemaphore(3, sem_id, 0);  /* Unlock */
                return 0;  /* Successfully acquired */
            }
            /* Critical section end */
            
            Psemaphore(3, sem_id, 0);  /* Unlock */
            
            /* Semaphore not available, yield CPU and retry */
            /* In single-threaded mode, this gives other processes a chance */
            if (sched_yield() != 0) {
                /* If yield fails, do a brief sleep to avoid busy waiting */
                usleep(1000);  /* 1ms sleep */
            }
        }

    }

    return 0;
}

int sem_trywait(sem_t *sem) {

    errno = 0;

    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (pthread_is_multithreaded_np()) {
        short old_count = sem->count;

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
                errno = 0;  /* Ensure errno is clear on success */
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
        int32_t sem_id;
        int result;

        if (sem->sem_id[0] == '\0') {
            errno = EINVAL;
            return -1;
        }

        sem_id = sem_id_from_name(sem->sem_id);

        /* Use Psemaphore as mutex to protect critical section */
        result = Psemaphore(2, sem_id, 0);  /* Try to lock without waiting */
        if (result < 0) {
            errno = EAGAIN;
            return -1;
        }

        /* Critical section start */
        if (sem->count > 0) {
            sem->count--;
            Psemaphore(3, sem_id, 0);  /* Unlock */
            return 0;
        } else {
            Psemaphore(3, sem_id, 0);  /* Unlock */
            errno = EAGAIN;
            return -1;
        }
    }
    return 0;
}

int sem_post(sem_t *sem) {
    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (pthread_is_multithreaded_np()) {
        if (sys_p_thread_sync(THREAD_SYNC_SEM_POST, (long)sem, 0) < 0) {
            return -1;
        }
    } else {
        int32_t sem_id;
        int result;
        
        if (sem->sem_id[0] == '\0') {
            errno = EINVAL;
            return -1;
        }
        
        sem_id = sem_id_from_name(sem->sem_id);
        
        /* Use Psemaphore as mutex to protect critical section */
        result = Psemaphore(2, sem_id, -1);  /* Lock */
        if (result < 0) {
            errno = EINVAL;
            return -1;
        }

        /* Critical section start */
        if (sem->count < SEM_VALUE_MAX) {
            sem->count++;
            // printf("sem_post: sem->count = %d\n", sem->count);
            Psemaphore(3, sem_id, 0);  /* Unlock */
            return 0;
        } else {
            Psemaphore(3, sem_id, 0);  /* Unlock */
            errno = EOVERFLOW;
            return -1;
        }
    }
    return 0;
}

int sem_getvalue(sem_t *sem, int *sval) {
    if (!sem || !sval) {
        errno = EINVAL;
        return -1;
    }
    
    *sval = sem->count;
    return 0;
}

int sem_destroy(sem_t *sem) {
    if (!sem) {
        errno = EINVAL;
        return -1;
    }

    if (pthread_is_multithreaded_np()) {
        /* Multithreaded mode: use pthread semaphore implementation */
        if (sem->wait_queue) {
            errno = EBUSY;
            return -1;
        }
        
        sem->count = 0;
        sem->wait_queue = NULL;
        strcpy(sem->sem_id, "");
    } else {
        /* Single-threaded mode: use original implementation */
        int32_t sem_id;
        
        if (sem->sem_id[0] == '\0') {
            errno = EINVAL;
            return -1;
        }

        sem_id = name_to_sem_id(sem->sem_id); 
        if (Psemaphore(1, sem_id, 0) < 0) {
            /* Still clean up even if Psemaphore fails */
            strcpy(sem->sem_id, "");
            return -1;
        }

        strcpy(sem->sem_id, "");
        sem->count = 0;
    }

    return 0;
}

/* Helper function to convert timespec to milliseconds timeout for Psemaphore */
static int32_t timespec_to_timeout_ms(const struct timespec *abs_timeout) {
    struct timespec current_time;
    int64_t timeout_ms;
    
    if (!abs_timeout) {
        return -1; /* No timeout */
    }
    
    /* Get current time */
    if (clock_gettime(CLOCK_REALTIME, &current_time) != 0) {
        return -1; /* Error getting time, use no timeout */
    }
    
    /* Calculate timeout in milliseconds */
    timeout_ms = (abs_timeout->tv_sec - current_time.tv_sec) * 1000LL +
                 (abs_timeout->tv_nsec - current_time.tv_nsec) / 1000000LL;
    
    if (timeout_ms <= 0) {
        return 0; /* Already expired, return immediately */
    }
    
    if (timeout_ms > INT32_MAX) {
        return INT32_MAX; /* Cap at maximum int32_t value */
    }
    
    return (int32_t)timeout_ms;
}

/* Helper function to convert timespec to milliseconds timeout for specific clock */
static int32_t timespec_to_timeout_ms_clock(const struct timespec *abs_timeout, clockid_t clock_id) {
    struct timespec current_time;
    int64_t timeout_ms;
    
    if (!abs_timeout) {
        return -1; /* No timeout */
    }
    
    /* Get current time using specified clock */
    if (clock_gettime(clock_id, &current_time) != 0) {
        errno = EINVAL; /* Invalid clock ID or other error */
        return -2; /* Error indicator */
    }
    
    /* Calculate timeout in milliseconds */
    timeout_ms = (abs_timeout->tv_sec - current_time.tv_sec) * 1000LL +
                 (abs_timeout->tv_nsec - current_time.tv_nsec) / 1000000LL;
    
    if (timeout_ms <= 0) {
        return 0; /* Already expired, return immediately */
    }
    
    if (timeout_ms > INT32_MAX) {
        return INT32_MAX; /* Cap at maximum int32_t value */
    }
    
    return (int32_t)timeout_ms;
}

/* Helper function to check if absolute timeout has expired */
static int timeout_expired(const struct timespec *abs_timeout, clockid_t clock_id) {
    struct timespec current_time;
    
    if (!abs_timeout) {
        return 0; /* No timeout means never expired */
    }
    
    if (clock_gettime(clock_id, &current_time) != 0) {
        return 1; /* Error getting time, assume expired */
    }
    
    if (current_time.tv_sec > abs_timeout->tv_sec) {
        return 1;
    }
    
    if (current_time.tv_sec == abs_timeout->tv_sec && 
        current_time.tv_nsec >= abs_timeout->tv_nsec) {
        return 1;
    }
    
    return 0;
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
    int32_t timeout_ms;
    
    /* Reset errno at the start of each call */
    errno = 0;
    
    if (!sem) {
        errno = EINVAL;
        return -1;
    }
    
    if (!abs_timeout) {
        errno = EINVAL;
        return -1;
    }
    
    /* Validate timespec */
    if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000) {
        errno = EINVAL;
        return -1;
    }
    
    /* Check if timeout already expired */
    if (timeout_expired(abs_timeout, CLOCK_REALTIME)) {
        errno = ETIMEDOUT;
        return -1;
    }
    
    if (pthread_is_multithreaded_np()) {
        /* Multithreaded mode: polling approach with atomic operations */
        const int POLL_INTERVAL_MS = 2; /* 1ms polling interval */
        short old_count;
        
        while (!timeout_expired(abs_timeout, CLOCK_REALTIME)) {
            old_count = sem->count;
            
            /* Try to atomically decrement if count > 0 */
            while (old_count > 0) {
                if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&sem->count, 
                                      (long)old_count, (long)(old_count - 1)) == 1) {
                    /* Successfully acquired semaphore */
                    errno = 0;
                    return 0;
                }
                /* CAS failed, re-read current value */
                old_count = sem->count;
            }
            
            /* Semaphore not available, yield and sleep briefly */
            sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
            msleep(POLL_INTERVAL_MS);
        }
        
        /* Timeout expired */
        errno = ETIMEDOUT;
        return -1;
        
    } else {
        /* Single-threaded mode: use original implementation with timeout */
        int32_t sem_id;
        int result;

        if (sem->sem_id[0] == '\0') {
            errno = EINVAL;
            return -1;
        }
        
        timeout_ms = timespec_to_timeout_ms(abs_timeout);
        if (timeout_ms == -2) {
            /* Error in timespec_to_timeout_ms, errno already set */
            return -1;
        }
        
        sem_id = sem_id_from_name(sem->sem_id);
        
        /* Try to acquire semaphore with timeout */
        if (timeout_ms == 0) {
            /* Timeout already expired - try once without waiting */
            result = Psemaphore(2, sem_id, 0);
        } else {
            /* Wait with timeout */
            result = Psemaphore(2, sem_id, timeout_ms);
        }
        
        if (result < 0) {
            if (result == EACCDN) {
                errno = ETIMEDOUT;
            } else {
                errno = EINVAL;
            }
            return -1;
        }
        
        sem->count--;
        
        /* Release semaphore immediately if more count available */
        if (sem->count > 0) {
            Psemaphore(3, sem_id, 0);
        }
    }
    
    return 0;
}

int sem_clockwait(sem_t *sem, clockid_t clock_id, const struct timespec *abs_timeout) {
    int32_t timeout_ms;
    
    /* Reset errno at the start of each call */
    errno = 0;
    
    if (!sem) {
        errno = EINVAL;
        return -1;
    }
    
    if (!abs_timeout) {
        errno = EINVAL;
        return -1;
    }
    
    /* Validate timespec */
    if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000) {
        errno = EINVAL;
        return -1;
    }
    
    /* Validate clock_id - mintlib may have limited clock support */
    switch (clock_id) {
        case CLOCK_REALTIME:
#ifdef CLOCK_MONOTONIC
        case CLOCK_MONOTONIC:
#endif
            break;
        default:
            errno = EINVAL;
            return -1;
    }
    
    /* Check if timeout already expired */
    if (timeout_expired(abs_timeout, clock_id)) {
        errno = ETIMEDOUT;
        return -1;
    }
    
    if (pthread_is_multithreaded_np()) {
        /* Multithreaded mode: polling approach with atomic operations */
        const int POLL_INTERVAL_MS = 2; /* 1ms polling interval */
        short old_count;
        
        while (!timeout_expired(abs_timeout, clock_id)) {
            old_count = sem->count;
            
            /* Try to atomically decrement if count > 0 */
            while (old_count > 0) {
                if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&sem->count, 
                                      (long)old_count, (long)(old_count - 1)) == 1) {
                    /* Successfully acquired semaphore */
                    errno = 0;
                    return 0;
                }
                /* CAS failed, re-read current value */
                old_count = sem->count;
            }
            
            /* Semaphore not available, yield and sleep briefly */
            sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
            msleep(POLL_INTERVAL_MS);
        }
        
        /* Timeout expired */
        errno = ETIMEDOUT;
        return -1;
        
    } else {
        /* Single-threaded mode: use original implementation with timeout */
        int32_t sem_id;
        int result;

        if (sem->sem_id[0] == '\0') {
            errno = EINVAL;
            return -1;
        }
        
        timeout_ms = timespec_to_timeout_ms_clock(abs_timeout, clock_id);
        if (timeout_ms == -2) {
            /* Error in timespec_to_timeout_ms_clock, errno already set */
            return -1;
        }
        
        sem_id = sem_id_from_name(sem->sem_id);
        
        /* Try to acquire semaphore with timeout */
        if (timeout_ms == 0) {
            /* Timeout already expired - try once without waiting */
            result = Psemaphore(2, sem_id, 0);
        } else {
            /* Wait with timeout */
            result = Psemaphore(2, sem_id, timeout_ms);
        }
        
        if (result < 0) {
            if (result == EACCDN) {
                errno = ETIMEDOUT;
            } else {
                errno = EINVAL;
            }                
            return -1;
        }
        
        sem->count--;
        
        /* Release semaphore immediately if more count available */
        if (sem->count > 0) {
            Psemaphore(3, sem_id, 0);
        }
    }
    
    return 0;
}