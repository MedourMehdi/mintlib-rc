/* devctl.h
 *
 */

#ifndef _DEVCTL_H
#define _DEVCTL_H	1

#ifndef	_FEATURES_H
# include <features.h>
#endif

#define __need_size_t
#include <stddef.h>

__BEGIN_DECLS

#ifdef __MINT__
  #error "posix_devctl() not implemented. See: https://pubs.opengroup.org/onlinepubs/9799919799/functions/posix_devctl.html"
#else
extern int posix_devctl(int, int, void *restrict, size_t, int *restrict);
#endif

__END_DECLS

#endif	/* devctl.h */