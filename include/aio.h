#ifndef _AIO_H
#define _AIO_H

#include <features.h>
#include <signal.h>	/* for sigevent (struct) and sigval (union) */
#include <time.h>	/* for struct timespec */
#include <sys/types.h>	/* for off_t, pthread_attr, size_t and ssize_t */

#ifdef __MINT__
#error "aio functions not implemented. See: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/aio.h.html"
#endif

__BEGIN_DECLS

struct aiocb {
	int aio_fildes, aio_lio_opcode, aio_reqprio;
	volatile void *aio_buf;
	size_t aio_nbytes;
	struct sigevent aio_sigevent;
	void *__td;
	int __lock[2];
	volatile int __err;
	ssize_t __ret;
	off_t aio_offset;
	void *__next, *__prev;
	char __dummy4[32-2*sizeof(void *)];
};

#define AIO_CANCELED 0
#define AIO_NOTCANCELED 1
#define AIO_ALLDONE 2

#define LIO_READ 0
#define LIO_WRITE 1
#define LIO_NOP 2

#define LIO_WAIT 0
#define LIO_NOWAIT 1

int aio_read(struct aiocb *);
int aio_write(struct aiocb *);
int aio_error(const struct aiocb *);
ssize_t aio_return(struct aiocb *);
int aio_cancel(int, struct aiocb *);
int aio_suspend(const struct aiocb *const [], int, const struct timespec *);
int aio_fsync(int, struct aiocb *);

int lio_listio(int, struct aiocb *__restrict const *__restrict, int, struct sigevent *__restrict);

__END_DECLS

#endif /* !_AIO_H */