/*
 *	Definitions for the ioctl() commands on sockets.
 *
 *	09/25/93, kay roemer.
 *	Updated: Added RFC 3493 interface name/index operations
 */

#ifndef _SOCKIOS_H
#define _SOCKIOS_H

/* socket-level I/O control calls */
#define SIOCGLOWAT	(('S' << 8) | 1)
#define SIOCSLOWAT	(('S' << 8) | 2)
#define SIOCGHIWAT	(('S' << 8) | 3)
#define SIOCSHIWAT	(('S' << 8) | 4)
#define SIOCSPGRP	(('S' << 8) | 5)
#define SIOCGPGRP	(('S' << 8) | 6)
#define SIOCATMARK	(('S' << 8) | 7)
/* (('S' << 8) | 101) reserved for MagiCNet */

/* socket configuration controls */
#define SIOCGIFNAME	(('S' << 8) | 10)	/* get iface name (original MiNT) */
#define SIOCSIFLINK	(('S' << 8) | 11)	/* connect iface to device */
#define SIOCGIFCONF	(('S' << 8) | 12)	/* get iface list */
#define SIOCGIFFLAGS	(('S' << 8) | 13)	/* get flags */
#define SIOCSIFFLAGS	(('S' << 8) | 14)	/* set flags */
#define SIOCGIFADDR	(('S' << 8) | 15)	/* get PA address */
#define SIOCSIFADDR	(('S' << 8) | 16)	/* set PA address */
#define SIOCGIFDSTADDR	(('S' << 8) | 17)	/* get remote PA address */
#define SIOCSIFDSTADDR	(('S' << 8) | 18)	/* set remote PA address */
#define SIOCGIFBRDADDR	(('S' << 8) | 19)	/* get broadcast PA address */
#define SIOCSIFBRDADDR	(('S' << 8) | 20)	/* set broadcast PA address */
#define SIOCGIFNETMASK	(('S' << 8) | 21)	/* get network PA mask */
#define SIOCSIFNETMASK	(('S' << 8) | 22)	/* set network PA mask */
#define SIOCGIFMETRIC	(('S' << 8) | 23)	/* get metric */
#define SIOCSIFMETRIC	(('S' << 8) | 24)	/* set metric */
#define SIOCSLNKFLAGS	(('S' << 8) | 25)	/* set link level flags */
#define SIOCGLNKFLAGS	(('S' << 8) | 26)	/* get link level flags */
#define SIOCGIFMTU	(('S' << 8) | 27)	/* get MTU size */
#define SIOCSIFMTU	(('S' << 8) | 28)	/* set MTU size */
#define SIOCGIFSTATS	(('S' << 8) | 29)	/* get interface statistics */

/* routing table calls */
#define SIOCADDRT	(('S' << 8) | 30)	/* add routing table entry */
#define SIOCDELRT	(('S' << 8) | 31)	/* delete routing table entry */

/* 
 * RFC 3493 Interface name/index operations (added for POSIX compliance)
 * Note: These are different from the original SIOCGIFNAME above
 */
#define SIOCGIFNAME_IFREQ	(('S' << 8) | 32)	/* get interface name by index (RFC 3493) */
#define SIOCGIFINDEX		(('S' << 8) | 33)	/* get interface index by name (RFC 3493) */

/* 
 * Reserved range for future interface operations: 34-39
 * Please update this comment when adding new interface IOCTLs
 */

/* ARP cache control calls */
#define SIOCDARP	(('S' << 8) | 40)	/* delete ARP table entry */
#define SIOCGARP	(('S' << 8) | 41)	/* get ARP table entry */
#define SIOCSARP	(('S' << 8) | 42)	/* set ARP table entry */

/* hardware address control */
#define SIOCSIFHWADDR	(('S' << 8) | 49)	/* set hardware address */
#define SIOCGIFHWADDR	(('S' << 8) | 50)	/* get hardware address */
#define SIOCGLNKSTATS	(('S' << 8) | 51)	/* get link statistics */
#define SIOCSIFOPT	(('S' << 8) | 52)	/* set interface option */

/*
 * Compatibility definitions for RFC 3493 functions
 * Only define the alias if GNU extensions are requested AND
 * the application doesn't seem to be using the original MiNT SIOCGIFNAME
 */
#ifdef __USE_GNU
/* Provide an alias for the RFC 3493 name-by-index operation */
#define SIOCGIFNAME_RFC3493	SIOCGIFNAME_IFREQ
#endif

#endif /* _SOCKIOS_H */