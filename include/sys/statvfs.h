#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#include <bits/statvfs.h>

int statvfs (const char *, struct statvfs *) __THROW __nonnull ((1, 2));
int fstatvfs (int, struct statvfs *) __THROW __nonnull ((2));

__END_DECLS

#endif
