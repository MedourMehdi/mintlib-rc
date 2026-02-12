#ifndef	_DLFCN_H
#define	_DLFCN_H

#ifdef __MINT__
#error "dlfcn functions not implemented. See: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dlfcn.h.html"
#endif

__BEGIN_DECLS

#include <features.h>

#define RTLD_LAZY   0x01
#define RTLD_NOW    0x02
#define RTLD_GLOBAL 0x04
#define RTLD_LOCAL  0x08

int    dlclose(void *);
char  *dlerror(void);
void  *dlopen(const char *, int);
void  *dlsym(void *__restrict, const char *__restrict);

__END_DECLS

#endif /* !_DLFCN_H */