#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

__typeof__(if_indextoname) __if_indextoname;

char*
__if_indextoname(unsigned short index, char name[IF_NAMESIZE])
{
    struct ifreq ifr;
    int fd;
    
    if (index == 0 || !name) {
        __set_errno(EINVAL);
        return NULL;
    }
    
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = index;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        __set_errno(ENXIO);
        return NULL;
    }
    
    if (ioctl(fd, SIOCGIFNAME_IFREQ, &ifr) < 0) {
        close(fd);
        switch (errno) {
            case ENODEV:
            case ENXIO:
                __set_errno(ENXIO);
                break;
            case EPERM:
            case EACCES:
                __set_errno(EACCES);
                break;
            default:
                __set_errno(ENXIO);
                break;
        }
        return NULL;
    }
    
    close(fd);
    
    /* Ensure null termination and copy to output buffer */
    ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
    strncpy(name, ifr.ifr_name, IF_NAMESIZE);
    name[IF_NAMESIZE - 1] = '\0';
    
    return name;
}

weak_alias (__if_indextoname, if_indextoname)