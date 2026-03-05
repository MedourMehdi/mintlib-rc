#include <sys/statvfs.h>
#include <sys/statfs.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <mintbind.h>    
#include <mint/dcntl.h>  
#include <unistd.h>
#include "fstype.h"
#include "lib.h"         

/* Forward declaration of internal statfs (from your statfs.c) */
extern int __statfs(const char *path, struct statfs *buf);

/* Convert internal 'struct statfs' to standard 'struct statvfs' */
static void __fixup(struct statvfs *out, const struct statfs *in, long fstype)
{
    memset(out, 0, sizeof(struct statvfs));
    out->f_bsize   = in->f_bsize;
    out->f_frsize  = in->f_bsize;
    out->f_blocks  = in->f_blocks;
    out->f_bfree   = in->f_bfree;
    out->f_bavail  = in->f_bavail;
    out->f_files   = in->f_files;
    out->f_ffree   = in->f_ffree;
    out->f_favail  = in->f_ffree;
    out->f_fsid    = in->f_fsid;
    out->f_flag    = 0;
    out->f_namemax = __fstype_namemax(fstype);
}

__typeof__(statvfs) __statvfs;

int __statvfs(const char *path, struct statvfs *buf)
{
	struct statfs kbuf;
	if (__statfs(path, &kbuf) < 0) return -1;
	__fixup(buf, &kbuf, kbuf.f_type);
	return 0;
}
weak_alias (__statvfs, statvfs)

__typeof__(fstatvfs) __fstatvfs;

#if __GNUC_PREREQ(7, 0)
# pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

int __fstatvfs(int fd, struct statvfs *buf)
{
    struct stat st;
    struct statfs kbuf;
    char savedcwd[PATH_MAX];
    long r;

    if (!buf) {
        __set_errno(EFAULT);
        return -1;
    }

    /* Validates fd — gives EBADF for free */
    if (fstat(fd, &st) < 0)
        return -1;

    if (S_ISDIR(st.st_mode)) {
        /*
         * Directory fd: fchdir into it, stat ".", restore CWD.
         * We must save CWD first and always restore it.
         */
        if (getcwd(savedcwd, sizeof(savedcwd)) == NULL) {
            __set_errno(EACCES);
            return -1;
        }

        r = fchdir(fd);
        if (r < 0) {
            /* CWD still intact — safe to return */
            return -1;
        }

        r = __statfs(".", &kbuf);
        /* Always restore, regardless of __statfs result */
        chdir(savedcwd);
    } else {
        /*
         * Regular file (or pipe, device, etc.):
         * Filesystem stats are device-wide, so if CWD is on the
         * same device we can use "." directly — no chdir needed.
         * If on a different device we have no path to the fd's
         * filesystem (no F_GETPATH on MiNT), so we fall back to
         * "." and return what we can. This is a known limitation.
         */
        r = __statfs(".", &kbuf);
    }

    if (r < 0)
        return -1;

    __fixup(buf, &kbuf, kbuf.f_type);
    return 0;
}
weak_alias(__fstatvfs, fstatvfs)
