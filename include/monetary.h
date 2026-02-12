#ifndef _MONETARY_H
#define _MONETARY_H

#ifdef __MINT__
#error "strfmon() and strfmon_l() not implemented. See: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/monetary.h.html"
#endif 

__BEGIN_DECLS

#include <features.h>

#define __NEED_ssize_t
#define __NEED_size_t
#define __NEED_locale_t

#include <bits/types.h>

ssize_t strfmon(char *__restrict, size_t, const char *__restrict, ...);
ssize_t strfmon_l(char *__restrict, size_t, locale_t, const char *__restrict, ...);

__END_DECLS

#endif /* !_MONETARY_H */