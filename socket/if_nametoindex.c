#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>

__typeof__(if_nametoindex) __if_nametoindex;

unsigned short 
__if_nametoindex(const char *name)
{
    struct ifreq ifr;
    int fd;
	int ret = 0;
    
    if (!name) {
		printf("name is null\n");
        __set_errno(EINVAL);
        return 0;
    }
    
    /* Check name length to prevent buffer overflow */
    if (strlen(name) >= IF_NAMESIZE) {
		printf("name too long\n");
        __set_errno(ENAMETOOLONG);
        return 0;
    }
    
    /* Initialize and copy name */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, name, IF_NAMESIZE - 1);
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        __set_errno(ENODEV);
        return 0;
    }
    
	ret = ioctl(fd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        close(fd);
        /* More specific error handling based on common failure modes */
        switch (errno) {
            case ENODEV:    /* No such device */
            case ENXIO:     /* No such device or address */
                __set_errno(ENODEV);
                break;
            case EPERM:     /* Operation not permitted */
            case EACCES:    /* Permission denied */
                __set_errno(EACCES);
                break;
            default:
                __set_errno(ENODEV);  /* Default for interface-related errors */
                break;
        }
        return 0;
    }
	
    close(fd);
    return ifr.ifr_ifindex;
}

weak_alias (__if_nametoindex, if_nametoindex)