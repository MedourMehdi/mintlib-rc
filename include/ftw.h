/*
**  <FTW.H>
**  Header values for the third parameter to the user-supplied function
**  for ftw().
**
** 2026-02-12 Updated with additional constants. Caution: ftw is deprecated. Use nftw instead.
**            See: https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/ftw.h.html
*/

#ifndef _FTW_H
#define _FTW_H

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

struct FTW {
	int	base;
	int	level;
};

/* Third argument of ntfw() */
#define FTW_F		0	/* A normal file			*/
#define FTW_D		1	/* A directory				*/
#define FTW_DNR		2	/* Something opendir(3) failed on	*/
#define FTW_NS		3	/* Something stat(2) failed on		*/
#define FTW_DP		4	/* A directory with subdirectories visited	*/
#define FTW_SL		5	/* A symbolic link			*/
#define FTW_SLN		6	/* A symbolic link that names a nonexistent file*/

/* Fourth argument of nftw() */
#define FTW_PHYS	0x01
#define FTW_MOUNT	0x02
#define FTW_XDEV	0x04
#define FTW_DEPTH	0x08
#define FTW_CHDIR	0x10

extern int ftw (const char *path, int (*fn)(const char *, const struct stat *, int), int param);

#ifndef __MINT__
extern int nftw (const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int param3, int param4);
#endif

__END_DECLS

#endif /* _FTW_H */
