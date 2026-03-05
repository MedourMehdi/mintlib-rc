/*
 * mint/fstype.h -- MiNTLib internal helpers for MiNT filesystem types.
 *
 * These helpers translate MiNT filesystem type codes (as returned by
 * Dcntl(FS_INFO, ...)) into POSIX-relevant properties.
 *
 * This is an INTERNAL header — do not install or expose publicly.
 * Include only from mintlib source files that need fs-type knowledge.
 *
 * Written for the MiNTLib project.
 * Placed in the Public Domain.
 */

#ifndef _MINT_FSTYPE_H
#define _MINT_FSTYPE_H

#include <limits.h>
#include <mint/dcntl.h>

/*
 * __fstype_namemax:
 *
 * Return the maximum filename length for a given MiNT filesystem type.
 * This is the value to use for f_namemax in statvfs and _PC_NAME_MAX
 * in pathconf/fpathconf when the kernel does not provide the answer
 * directly via Dpathconf or FS_USAGE.
 */
static __inline__ long
__fstype_namemax (long type)
{
    long major = type & 0xffff0000L;

    switch (major)
    {
        case _MAJOR_OLDTOS:
        case _MAJOR_FAT:
            return 12;      /* 8.3: 8 + dot + 3 */

        case _MAJOR_VFAT:
            return 255;     /* VFAT long file names */

        case _MAJOR_MINIX:
            return 30;      /* Minix v1 hard limit */

        case _MAJOR_HSIERRA:
        case _MAJOR_ISO9660:
            return 31;      /* ISO9660 level 1 */

        case _MAJOR_JOLIET:
            return 255;     /* Joliet Unicode names */

        case _MAJOR_HFS:
            return 31;      /* HFS limit */

        case _MAJOR_EXT2:
            return 255;     /* ext2 NAME_MAX */

        case _MAJOR_RAMFS:
            return 255;

        case _MAJOR_STONX:
            return 255;     /* host Linux filesystem */

        default:
            return NAME_MAX;
    }
}

/*
 * __fstype_pathmax:
 *
 * Return the maximum full pathname length for a given MiNT filesystem
 * type. Used for _PC_PATH_MAX in pathconf/fpathconf.
 *
 * Note: this is the filesystem-imposed limit, not the kernel buffer
 * size. On MiNT, PATH_MAX (1024) is the kernel buffer; TOS/FAT
 * filesystems have a much lower practical limit of 128 chars.
 */
static __inline__ long
__fstype_pathmax (long type)
{
    long major = type & 0xffff0000L;

    switch (major)
    {
        case _MAJOR_OLDTOS:
        case _MAJOR_FAT:
        case _MAJOR_VFAT:
            return 128;     /* TOS/FAT practical path limit */

        case _MAJOR_HSIERRA:
        case _MAJOR_ISO9660:
            return 256;     /* ISO9660 full path limit */

        case _MAJOR_JOLIET:
        case _MAJOR_HFS:
        case _MAJOR_MINIX:
        case _MAJOR_EXT2:
        case _MAJOR_RAMFS:
        case _MAJOR_STONX:
            return PATH_MAX;

        default:
            return PATH_MAX;
    }
}

/*
 * __fstype_no_trunc:
 *
 * Return 1 if the filesystem returns an error when a filename exceeds
 * the maximum length (POSIX _PC_NO_TRUNC semantics).
 * Return 0 if the filesystem silently truncates long names (TOS/FAT).
 */
static __inline__ int
__fstype_no_trunc (long type)
{
    long major = type & 0xffff0000L;

    switch (major)
    {
        case _MAJOR_OLDTOS:
        case _MAJOR_FAT:
        case _MAJOR_VFAT:
            return 0;   /* silently truncates */

        case _MAJOR_MINIX:
        case _MAJOR_EXT2:
        case _MAJOR_RAMFS:
        case _MAJOR_HSIERRA:
        case _MAJOR_ISO9660:
        case _MAJOR_JOLIET:
        case _MAJOR_HFS:
        case _MAJOR_STONX:
            return 1;   /* errors on overflow */

        default:
            return 1;   /* safe assumption */
    }
}

/*
 * __fstype_case_insensitive:
 *
 * Return 1 if the filesystem is case-insensitive (_MINT_PC_NAME_CASE),
 * 0 if it is case-sensitive.
 *
 * Note: VFAT stores names case-insensitively even though it preserves
 * the original case on disk — lookups are case-insensitive so it
 * returns 1 here.
 */
static __inline__ int
__fstype_case_insensitive (long type)
{
    long major = type & 0xffff0000L;

    switch (major)
    {
        case _MAJOR_OLDTOS:
        case _MAJOR_FAT:
        case _MAJOR_VFAT:
        case _MAJOR_HSIERRA:
        case _MAJOR_ISO9660:
        case _MAJOR_JOLIET:
        case _MAJOR_HFS:
            return 1;   /* case-insensitive */

        case _MAJOR_MINIX:
        case _MAJOR_EXT2:
        case _MAJOR_RAMFS:
        case _MAJOR_STONX:
            return 0;   /* case-sensitive */

        default:
            return 0;   /* assume sensitive if unknown */
    }
}

#endif /* _MINT_FSTYPE_H */
