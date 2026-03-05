/*
 * pathconf() for TOS/MiNT (kinda POSIXy)
 * Written by Dave Gymer and placed in the Public Domain
 *
 * NOTE: this file will have to be updated as and when MiNT gains
 * new Sysconf() and Dpathconf() variables, as will unistd.h, so that
 * TOS programs get the correct values back. (But who uses TOS? :-)
 */

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <mint/mintbind.h>
#include "fstype.h"
#include "lib.h"

#define UNLIMITED	(0x7fffffffL)

long
__pathconf (const char *_path, int var)
{
    long r;
    char pathbuf[PATH_MAX];
    char *path = (char *) _path;

    if (!__libc_unix_names)
    {
        path = pathbuf;
        _unx2dos (_path, path, sizeof (pathbuf));
    }

    /* Try the kernel first — it knows best for the actual filesystem */
    r = Dpathconf (path, var);
    if (r != -ENOSYS)
        return r;

    /*
     * Fallback for old kernels / bare TOS without Dpathconf.
     * Try Dcntl(FS_INFO) first for filesystem-specific answers,
     * then fall back to hardcoded TOS conservative defaults.
     */
    switch (var)
    {
        case _PC_LAST:
            return _MINT_PC_NAME_CASE;  /* highest define we know */

        case _PC_IOPEN_MAX:
            return 45;  /* TOS approximate */

        case _PC_LINK_MAX:
            return 1;   /* TOS has no hard links */

        case _PC_PATH_MAX:
        {
            struct fs_info info;
            if (Dcntl (FS_INFO, path, &info) == 0)
                return __fstype_pathmax (info.type);
            return 128; /* bare TOS fallback */
        }

        case _PC_NAME_MAX:
        {
            struct fs_info info;
            if (Dcntl (FS_INFO, path, &info) == 0)
                return __fstype_namemax (info.type);
            return 12;  /* bare TOS 8.3 fallback */
        }

        case _PC_PIPE_BUF:
            /* No pipes on bare TOS */
            __set_errno (EINVAL);
            return -1;

        case _PC_NO_TRUNC:
        {
            struct fs_info info;
            if (Dcntl (FS_INFO, path, &info) == 0)
                return __fstype_no_trunc (info.type);
            return 0;   /* bare TOS: truncates silently */
        }

        case _MINT_PC_NAME_CASE:
        {
            struct fs_info info;
            if (Dcntl (FS_INFO, path, &info) == 0)
                return __fstype_case_insensitive (info.type);
            return 1;   /* bare TOS default: case-insensitive */
        }

        default:
            __set_errno (EINVAL);
            return -1;
    }
}
weak_alias (__pathconf, pathconf)
