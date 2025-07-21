#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H	1

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEM_VALUE_MAX
#define SEM_VALUE_MAX 32767
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
    volatile short count;           /* Current semaphore count */
    struct thread *wait_queue;      /* Queue of threads waiting on this sem */
    /* Non threaded values */
    volatile short io_count;        /* Reference count for named sems */
    char sem_id[SEM_NAME_MAX + 1];  /* Fixed array instead of pointer */
} sem_t;

/* POSIX semaphore functions */
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);

/* Named semaphore functions */
sem_t *sem_open(const char* name, int oflag, ...);
//  sem_t *sem_open(const char* name, int oflag, mode_t mode, unsigned int value);
int sem_close(sem_t* sem);
int sem_unlink(const char* name);

/* Extended functions */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_clockwait(sem_t *sem, clockid_t clock_id, const struct timespec *abs_timeout);

#ifdef __cplusplus
}
#endif

#endif /* _SEMAPHORE_H */