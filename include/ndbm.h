#ifndef _COMPAT_NDBM_H_
#define	_COMPAT_NDBM_H_

#ifdef __MINT__
  #error "ndbm functions not implemented. See: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/ndbm.h.html"
#endif

#include <stddef.h> /* for size_t */
#include <sys/types.h> /* for mode_t */

#define	DBM_INSERT	0
#define	DBM_REPLACE	1

typedef void* DBM; /* __MINT__ not implemented */

typedef struct {
	void	*dptr;
	size_t	 dsize;
} datum;

__BEGIN_DECLS

int	 dbm_clearerr(DBM *);
void	 dbm_close(DBM *);
int	 dbm_delete(DBM *, datum);
int	 dbm_error(DBM *);
datum	 dbm_fetch(DBM *, datum);
datum	 dbm_firstkey(DBM *);
datum	 dbm_nextkey(DBM *);
DBM	 *dbm_open(const char *, int, mode_t);
int	 dbm_store(DBM *, datum, datum, int);

__END_DECLS

#endif /* !_COMPAT_NDBM_H_ */