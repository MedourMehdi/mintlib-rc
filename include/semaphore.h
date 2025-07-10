#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H	1

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEM_VALUE_MAX 32767
#define SEM_NAME_MAX    4
#define SEM_FAILED ((sem_t *) 0) 

/* FreeMiNT specific error codes */
#define EACCDN  -36
#define ENHNDL  -35
#define EPTHNF  -34
#define EFILNF  -33

typedef struct
{
    int max_count;
    int io_count;    
    char *sem_id;
} sem_t;

/* Internal helper functions */
char *gen_sem_id(int16_t length);
int32_t sem_id_from_name(const char *name);

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