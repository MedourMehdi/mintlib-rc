#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H	1

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEM_VALUE_MAX
#define SEM_VALUE_MAX   (__INT_MAX__)
#endif
#define SEM_NAME_MAX    4
#define SEM_FAILED ((sem_t *) 0) 

/* FreeMiNT specific error codes */
#define EACCDN  -36
#define ENHNDL  -35
#define EPTHNF  -34
#define EFILNF  -33

typedef struct
{
    /* Non threaded values */
    volatile unsigned short max_count;
    volatile unsigned short io_count;
    char *sem_id;
    /* pthread used values */
    volatile unsigned short count;
    struct thread *wait_queue;
} sem_t;

/* Internal function to check if we're in multithreaded mode */
int _sem_is_multithreaded(void);

/* POSIX semaphore functions */
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);

/* Named semaphore functions */
sem_t *sem_open(const char* name, int oflag, ...);
int sem_close(sem_t* sem);
int sem_unlink(const char* name);

/* Extended functions */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_clockwait(sem_t *sem, clockid_t clock_id, const struct timespec *abs_timeout);

/* Common wait implementation */
int sem_waitcommon(sem_t *sem, int blocking, const struct timespec *abs_timeout, clockid_t clock_id);

#ifdef __cplusplus
}
#endif

#endif /* _SEMAPHORE_H */