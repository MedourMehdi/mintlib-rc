#ifndef _SEMAPHORE_PRIV_H
#define _SEMAPHORE_PRIV_H

/* Erreurs spécifiques à MiNT non définies dans mintlib */
#define EACCDN  -36
#define ENHNDL  -35
#define EPTHNF  -34
#define EFILNF  -33

int32_t sem_id_from_name(const char *name);
int32_t name_to_sem_id(const char *name);

/*
 * Psemaphore - Non-counting semaphore implementation
 * 
 * DESCRIPTION:
 *   Implements non-counting semaphores for process synchronization. Only one 
 *   process can access a semaphore at a time. Useful for synchronizing access 
 *   to shared memory data structures between subprocesses.
 * 
 * PARAMETERS:
 *   id      - Semaphore identifier (LONGWORD, may contain 4 ASCII chars)
 *             Names starting with '_' are reserved for the operating system
 *   mode    - Operation mode (see MODE VALUES below)
 *   timeout - Timeout value in milliseconds (only used in mode 2)
 *             0: Return immediately
 *             -1: No timeout (wait indefinitely)
 *             Other: Delay time in milliseconds before timeout
 * 
 * MODE VALUES:
 *   0 - Create semaphore with ID 'id' and grant ownership to calling process
 *   1 - Destroy semaphore with ID 'id' (only if owned by caller)
 *   2 - Request ownership of semaphore with ID 'id' (blocks until available 
 *       or timeout occurs)
 *   3 - Release ownership of semaphore with ID 'id' (only if owned by caller)
 * 
 * USAGE NOTES:
 *   - When a process creates a semaphore, it owns it immediately
 *   - Semaphore must be released before other processes can use it
 *   - Use function sparingly as creation reserves internal memory
 *   - All semaphores owned by a process are released automatically on termination
 *   - Semaphores are NOT destroyed automatically on process termination
 *   - Ownership cannot be inherited (e.g., with Pfork)
 * 
 * RETURN VALUES:
 *   E_OK  : No error has arisen
 *   ERROR : A semaphore was requested that the caller already owns
 *   EACCDN: An attempt was made to create a semaphore that already esists, the caller is not the owner of the semaphore or a timeout has occurred
 *   ERANGE: The specified semaphore does not exist 
 * 
 * WARNINGS:
 *   - If a process creates a semaphore and crashes, the semaphore remains
 *     in the system and cannot be removed
 *   - Blocked processes waiting for a destroyed semaphore receive ERANGE
 */

#endif