/*  fpathconf.c -- MiNTLib.
    Copyright (C) 2001 Frank Naumann <fnaumann@freemint.de>
    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <mint/dcntl.h>
#include <sys/statvfs.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include "fstype.h"
#include <mint/mintbind.h>
#include "lib.h"

/* Internal helper: get MiNT fs type for an open fd */
static long
__fstype_from_fd (int fd)
{
    struct stat st;
    struct statfs kbuf;

    if (fstat (fd, &st) < 0)
        return -1;

    if (S_ISDIR (st.st_mode))
    {
        char savedcwd[PATH_MAX];
        long r;

        if (getcwd (savedcwd, sizeof (savedcwd)) == NULL)
            return -1;

        if (fchdir (fd) < 0)
            return -1;

        r = statfs (".", &kbuf);
        chdir (savedcwd);

        if (r < 0)
            return -1;
    }
    else
    {
        if (statfs (".", &kbuf) < 0)
            return -1;
    }

    return kbuf.f_type;
}

long
__fpathconf (int fd, int var)
{
    switch (var)
    {
        case _PC_IOPEN_MAX:
            return OPEN_MAX;

        case _PC_LINK_MAX:
            return LINK_MAX;

        case _PC_PATH_MAX:
        {
            long type = __fstype_from_fd (fd);
            if (type < 0)
                return -1;
            return __fstype_pathmax (type);
        }

        case _PC_NAME_MAX:
        {
            struct statvfs sv;
            if (fstatvfs (fd, &sv) < 0)
                return -1;
            return (long) sv.f_namemax;
        }

        case _PC_PIPE_BUF:
        {
            struct stat st;
            if (fstat (fd, &st) < 0)
                return -1;
            return st.st_blksize;
        }

        case _PC_NO_TRUNC:
        {
            long type = __fstype_from_fd (fd);
            if (type < 0)
                return -1;
            return __fstype_no_trunc (type);
        }

        case _MINT_PC_NAME_CASE:
        {
            long type = __fstype_from_fd (fd);
            if (type < 0)
                return -1;
            return __fstype_case_insensitive (type);
        }

        case _PC_CHOWN_RESTRICTED:
            return 1;

        case _PC_VDISABLE:
            return 0;

        case _PC_MAX_CANON:
        case _PC_MAX_INPUT:
            __set_errno (EINVAL);
            return -1;

        default:
            __set_errno (EINVAL);
            return -1;
    }
}
weak_alias (__fpathconf, fpathconf)
