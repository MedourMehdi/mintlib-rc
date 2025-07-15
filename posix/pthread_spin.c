#include <pthread.h>
#include "pthread_internal.h"
#include <mint/osbind.h>
#include <mint/dcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* Generate unique 8.3 format shared memory path */
static char* gen_spinlock_shm_path(void) {
    static unsigned int counter = 0;
    unsigned int unique_id = ((unsigned int)Pgetpid() << 16) | (counter++ & 0xFFFF);
    
    char *path = (char *)Mxalloc(22, MX_PREFTTRAM);
    if (!path) return NULL;
    
    snprintf(path, 22, "U:\\SHM\\%08X.SLK", unique_id);
    return path;
}

/* Structure for shared spinlock - in shared memory */
typedef struct {
    volatile int lock;
    volatile int refcount;
    volatile int initialized;
} pthread_spin_shm_t;

/* Local structure to track shared memory handles */
typedef struct {
    pthread_spin_shm_t *shm_ptr;  /* Fixed: was pthread_spin_local_t */
    long file_handle;
    char *shm_path;
    int is_creator;
} pthread_spin_local_t;

/* Structure for private spinlock */
typedef struct {
    volatile int lock;
    int magic;  // Magic number to identify private locks
} pthread_spin_private_t;

#define PRIVATE_SPINLOCK_MAGIC 0x50535043  // "PSPC"

/**
 * Initialize a spinlock
 */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
    if (!lock) return EINVAL;
    
    if (pshared == PTHREAD_PROCESS_PRIVATE) {
        // Allocate private spinlock structure
        pthread_spin_private_t *private_lock = (pthread_spin_private_t *)Mxalloc(sizeof(pthread_spin_private_t), MX_PREFTTRAM);
        if (!private_lock) return ENOMEM;
        
        private_lock->lock = 0;
        private_lock->magic = PRIVATE_SPINLOCK_MAGIC;
        
        *lock = (pthread_spinlock_t)private_lock;
        return 0;
    } 
    else if (pshared == PTHREAD_PROCESS_SHARED) {
        pthread_spin_local_t *local = NULL;
        pthread_spin_shm_t *shm_mem = NULL;
        long file_handle;
        long size = sizeof(pthread_spin_shm_t);
        int is_creator = 0;
        char *shm_path = gen_spinlock_shm_path();
        
        if (!shm_path) return ENOMEM;
        
        // Allocate local tracking structure
        local = (pthread_spin_local_t *)Mxalloc(sizeof(pthread_spin_local_t), MX_PREFTTRAM);
        if (!local) {
            Mfree(shm_path);
            return ENOMEM;
        }
        memset(local, 0, sizeof(pthread_spin_local_t));
        
        // Try to create the shared memory file
        file_handle = Fcreate(shm_path, 0);
        if (file_handle >= 0) {
            int perms;
            // We're the creator - set up shared memory
            is_creator = 1;
            
            // Allocate and initialize shared memory block
            shm_mem = (pthread_spin_shm_t *)Mxalloc(size, MX_PREFTTRAM);
            if (!shm_mem) {
                Fclose(file_handle);
                Fdelete(shm_path);
                Mfree(local);
                Mfree(shm_path);
                return ENOMEM;
            }
            
            // Initialize shared memory structure
            shm_mem->lock = 0;
            shm_mem->refcount = 1;
            shm_mem->initialized = 0;  // Mark not initialized yet
            
            // Set shared memory block
            if (Fcntl(file_handle, shm_mem, SHMSETBLK) < 0) {
                Mfree(shm_mem);
                Fclose(file_handle);
                Fdelete(shm_path);
                Mfree(local);
                Mfree(shm_path);
                return EIO;
            }
            
            // Set file permissions (user read/write)
            perms = S_IRUSR | S_IWUSR;
            if (Fcntl(file_handle, &perms, F_SETFL) < 0) {
                Mfree(shm_mem);
                Fclose(file_handle);
                Fdelete(shm_path);
                Mfree(local);
                Mfree(shm_path);
                return EIO;
            }
            
            // Memory barrier before marking as initialized
            asm volatile ("" ::: "memory");
            shm_mem->initialized = 1;
            
            local->shm_ptr = shm_mem;
        } else {
            // File exists - open existing shared memory
            file_handle = Fopen(shm_path, O_RDWR);
            if (file_handle < 0) {
                Mfree(local);
                Mfree(shm_path);
                return -file_handle;
            }
            
            // Get shared memory block
            if (Fcntl(file_handle, &shm_mem, SHMGETBLK) < 0) {
                Fclose(file_handle);
                Mfree(local);
                Mfree(shm_path);
                return EIO;
            }
            
            // Wait for initialization with yield
            while (!shm_mem->initialized) {
                sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
            }
            
            // Atomically increment reference count using kernel atomic
            sys_p_thread_atomic(THREAD_ATOMIC_INCREMENT, (long)&shm_mem->refcount, 0, 0);
            
            local->shm_ptr = shm_mem;
        }
        
        // Store tracking information
        local->file_handle = file_handle;
        local->is_creator = is_creator;
        local->shm_path = shm_path;
        
        *lock = (pthread_spinlock_t)local;
        return 0;
    }
    return EINVAL;
}

/**
 * Helper function to identify lock type
 */
static int is_private_lock(pthread_spinlock_t *lock) {
    pthread_spin_private_t *private_lock = (pthread_spin_private_t *)*lock;
    return (private_lock && private_lock->magic == PRIVATE_SPINLOCK_MAGIC);
}

/**
 * Destroy a spinlock
 */
int pthread_spin_destroy(pthread_spinlock_t *lock) {
    if (!lock || !*lock) return EINVAL;
    
    if (is_private_lock(lock)) {
        // Private lock
        pthread_spin_private_t *private_lock = (pthread_spin_private_t *)*lock;
        Mfree(private_lock);
        *lock = 0;
        return 0;
    } else {
        // Shared lock processing
        pthread_spin_local_t *local = (pthread_spin_local_t *)*lock;
        pthread_spin_shm_t *shm = local->shm_ptr;
        int should_free = 0;
        int new_ref = 0;
        
        if (!local || !shm) return EINVAL;
        
        // Atomically decrement reference count using kernel atomic
        new_ref = sys_p_thread_atomic(THREAD_ATOMIC_DECREMENT, (long)&shm->refcount, 0, 0);
        should_free = (local->is_creator && new_ref == 0);
        
        // Close file handle
        if (local->file_handle > 0) {
            Fclose(local->file_handle);
        }
        
        // Free resources if we're the creator and last reference
        if (should_free) {
            // Free shared memory block
            if (shm) {
                Mfree(shm);
            }
            
            // Delete shared memory file
            if (local->shm_path) {
                Fdelete(local->shm_path);
            }
        }
        
        // Free path and local structure
        if (local->shm_path) {
            Mfree(local->shm_path);
        }
        Mfree(local);
        
        *lock = 0;
        return 0;
    }
}

// Lock function using kernel atomic operations
int pthread_spin_lock(pthread_spinlock_t *lock) {
    if (!lock || !*lock) return EINVAL;
    
    if (is_private_lock(lock)) {
        // Private lock
        pthread_spin_private_t *private_lock = (pthread_spin_private_t *)*lock;
        while (1) {
            // Use kernel CAS to acquire lock - CAS returns 1 for success
            if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&private_lock->lock, 0, 1) == 1) {
                return 0;  // Successfully acquired lock
            }
            sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
        }
    } else {
        // Shared lock
        pthread_spin_local_t *local = (pthread_spin_local_t *)*lock;
        pthread_spin_shm_t *shm = local->shm_ptr;
        
        if (!local || !shm) return EINVAL;
        
        while (1) {
            // Use kernel CAS to acquire lock - CAS returns 1 for success
            if (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&shm->lock, 0, 1) == 1) {
                return 0;  // Successfully acquired lock
            }
            sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
        }
    }
}

// Trylock using kernel CAS
int pthread_spin_trylock(pthread_spinlock_t *lock) {
    if (!lock || !*lock) return EINVAL;
    
    if (is_private_lock(lock)) {
        // Private lock
        pthread_spin_private_t *private_lock = (pthread_spin_private_t *)*lock;
        // CAS returns 1 for success, 0 for failure
        return (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&private_lock->lock, 0, 1) == 1 ? 0 : EBUSY);
    } else {
        // Shared lock
        pthread_spin_local_t *local = (pthread_spin_local_t *)*lock;
        if (local && local->shm_ptr) {
            // CAS returns 1 for success, 0 for failure
            return (sys_p_thread_atomic(THREAD_ATOMIC_CAS, (long)&local->shm_ptr->lock, 0, 1) == 1 ? 0 : EBUSY);
        }
        return EINVAL;
    }
}

// Unlock function using kernel exchange
int pthread_spin_unlock(pthread_spinlock_t *lock) {
    if (!lock || !*lock) return EINVAL;
    
    if (is_private_lock(lock)) {
        // Private lock
        pthread_spin_private_t *private_lock = (pthread_spin_private_t *)*lock;
        sys_p_thread_atomic(THREAD_ATOMIC_EXCHANGE, (long)&private_lock->lock, 0, 0);
    } else {
        // Shared lock
        pthread_spin_local_t *local = (pthread_spin_local_t *)*lock;
        if (local && local->shm_ptr) {
            sys_p_thread_atomic(THREAD_ATOMIC_EXCHANGE, (long)&local->shm_ptr->lock, 0, 0);
        } else {
            return EINVAL;
        }
    }
    return 0;
}

// /**
//  * Attach to existing shared spinlock - corrected version
//  */
// int pthread_spin_attach(pthread_spinlock_t *lock, const char *shm_path) {
//     pthread_spin_local_t *local = NULL;
//     pthread_spin_shm_t *shm_mem = NULL;
//     long file_handle;
//     int timeout;
//     volatile int test_access;

//     if (!lock || !shm_path) return EINVAL;
    
//     // Allocate local structure
//     local = (pthread_spin_local_t*)Mxalloc(sizeof(pthread_spin_local_t), MX_PREFTTRAM);
//     if (!local) return ENOMEM;
//     memset(local, 0, sizeof(pthread_spin_local_t));
    
//     // Copy shared memory path
//     local->shm_path = (char*)Mxalloc(strlen(shm_path) + 1, MX_PREFTTRAM);
//     if (!local->shm_path) {
//         Mfree(local);
//         return ENOMEM;
//     }
//     strcpy(local->shm_path, shm_path);
    
//     // Open existing shared memory file
//     file_handle = Fopen(shm_path, O_RDWR);
//     if (file_handle < 0) {
//         Mfree(local->shm_path);
//         Mfree(local);
//         return -file_handle;
//     }
    
//     // Get the existing shared memory block (don't try to set a new one)
//     if (Fcntl(file_handle, &shm_mem, SHMGETBLK) < 0) {
//         Fclose(file_handle);
//         Mfree(local->shm_path);
//         Mfree(local);
//         return EIO;
//     }
    
//     // Validate that we got a valid pointer
//     if (!shm_mem) {
//         Fclose(file_handle);
//         Mfree(local->shm_path);
//         Mfree(local);
//         return EIO;
//     }
    
//     // Test memory accessibility before using it
//     __asm__ volatile (
//         "move.l %1, %%a0\n\t"
//         "move.l (%%a0), %0\n\t"
//         : "=d" (test_access)
//         : "a" (&shm_mem->initialized)
//         : "a0", "memory"
//     );
    
//     // Wait for initialization with timeout
//     timeout = 1000; // 1000 yields = reasonable timeout
//     while (!shm_mem->initialized && timeout-- > 0) {
//         sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
//     }
    
//     if (!shm_mem->initialized) {
//         Fclose(file_handle);
//         Mfree(local->shm_path);
//         Mfree(local);
//         return ETIMEDOUT;
//     }
    
//     // Atomically increment reference count using kernel atomic
//     sys_p_thread_atomic(THREAD_ATOMIC_INCREMENT, (long)&shm_mem->refcount, 0, 0);
    
//     local->shm_ptr = shm_mem;
//     local->file_handle = file_handle;
//     local->is_creator = 0;
    
//     *lock = (pthread_spinlock_t)local;
//     return 0;
// }

/**
 * File-based implementation that doesn't rely on shared memory mapping
 */
int pthread_spin_attach(pthread_spinlock_t *lock, const char *shm_path) {
    pthread_spin_local_t *local = NULL;
    pthread_spin_shm_t *shm_mem = NULL;
    long file_handle;
    long size = sizeof(pthread_spin_shm_t);

    if (!lock || !shm_path) return EINVAL;
    
    // Allocate local structure
    local = (pthread_spin_local_t*)Mxalloc(sizeof(pthread_spin_local_t), MX_PREFTTRAM);
    if (!local) return ENOMEM;
    memset(local, 0, sizeof(pthread_spin_local_t));
    
    // Copy shared memory path
    local->shm_path = (char*)Mxalloc(strlen(shm_path) + 1, MX_PREFTTRAM);
    if (!local->shm_path) {
        Mfree(local);
        return ENOMEM;
    }
    strcpy(local->shm_path, shm_path);
    
    // Open existing shared memory file
    file_handle = Fopen(shm_path, O_RDWR);
    if (file_handle < 0) {
        Mfree(local->shm_path);
        Mfree(local);
        return -file_handle;
    }
    
    // Allocate local memory for our copy of the shared structure
    shm_mem = (pthread_spin_shm_t *)Mxalloc(size, MX_PREFTTRAM);
    if (!shm_mem) {
        Fclose(file_handle);
        Mfree(local->shm_path);
        Mfree(local);
        return ENOMEM;
    }
    
    // Read current state from file
    Fseek(0, file_handle, SEEK_SET);
    if (Fread(file_handle, size, shm_mem) != size) {
        // Wait for parent to write initial state
        int retries = 100;
        while (retries-- > 0) {
            sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
            Fseek(0, file_handle, SEEK_SET);
            if (Fread(file_handle, size, shm_mem) == size) {
                break;
            }
        }
        if (retries <= 0) {
            Mfree(shm_mem);
            Fclose(file_handle);
            Mfree(local->shm_path);
            Mfree(local);
            return EIO;
        }
    }
    
    // Wait for initialization
    while (!shm_mem->initialized) {
        sys_p_thread_sync(THREAD_SYNC_YIELD, 0, 0);
        // Re-read from file to check initialization status
        Fseek(0, file_handle, SEEK_SET);
        Fread(file_handle, size, shm_mem);
    }
    
    // Increment reference count (we'll need to read-modify-write via file)
    shm_mem->refcount++;
    
    // Write back the updated structure
    Fseek(0, file_handle, SEEK_SET);
    Fwrite(file_handle, size, shm_mem);
    
    local->shm_ptr = shm_mem;
    local->file_handle = file_handle;
    local->is_creator = 0;
    
    *lock = (pthread_spinlock_t)local;
    return 0;
}
