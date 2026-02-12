#ifndef _MQUEUE_H
#define _MQUEUE_H

#ifdef __MINT__
#error "mqueue functions not implemented. See: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html"
#endif

__BEGIN_DECLS

#include <features.h>

#define __NEED_size_t
#define __NEED_ssize_t
#define __NEED_pthread_attr_t
#define __NEED_time_t
#define __NEED_struct_timespec
#include <bits/types.h>

typedef int mqd_t;
struct mq_attr {
	long mq_flags, mq_maxmsg, mq_msgsize, mq_curmsgs, __unused[4];
};
struct sigevent;

int mq_close(mqd_t);
int mq_getattr(mqd_t, struct mq_attr *);
int mq_notify(mqd_t, const struct sigevent *);
mqd_t mq_open(const char *, int, ...);
ssize_t mq_receive(mqd_t, char *, size_t, unsigned *);
int mq_send(mqd_t, const char *, size_t, unsigned);
int mq_setattr(mqd_t, const struct mq_attr *__restrict, struct mq_attr *__restrict);
ssize_t mq_timedreceive(mqd_t, char *__restrict, size_t, unsigned *__restrict, const struct timespec *__restrict);
int mq_timedsend(mqd_t, const char *, size_t, unsigned, const struct timespec *);
int mq_unlink(const char *);

__END_DECLS

#endif /* !_MQUEUE_H */