
#include <unistd.h>
#include <bits/local_lim.h>
#include <mint/osbind.h>
#include <mint/sysvars.h>
#include <errno.h>

#include <sys/sysinfo.h>

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif
#ifndef UNLIMITED
#define UNLIMITED	(0x7fffffffL)
#endif

/* 16MB boundary - Alt-RAM starts at 16MB on Atari systems */
#ifndef ALTRAM_START_BOUNDARY
#define ALTRAM_START_BOUNDARY 0x01000000L
#endif

/* Magic value that validates ramtop pointer */
#ifndef RAMVALID_MAGIC
#define RAMVALID_MAGIC 0x1357L
#endif

/* Helper functions to read system variables in supervisor mode */

static long read_phystop(void) {
    return *phystop;
}

static long read_ramtop(void) {
    return *ramtop;
}

static long read_ramvalid(void) {
    return *ramvalid;
}

/* Updated functions using the helpers */
static short is_there_altram(void){
    long ramtop_val, ramvalid_val;
    
    /* First check if ramvalid contains the magic value */
    ramvalid_val = Supexec(read_ramvalid);
    if (ramvalid_val < 0 || ramvalid_val != RAMVALID_MAGIC) {
        /* Supexec failed or ramvalid is not the magic value */
        return FALSE;
    }
    
    /* ramvalid is correct, now check ramtop */
    ramtop_val = Supexec(read_ramtop);
    if (ramtop_val < 0) {
        /* Supexec failed */
        return FALSE;
    }
    
    return (ramtop_val > ALTRAM_START_BOUNDARY) ? TRUE : FALSE;
}

static unsigned long int get_altram_value(void){
    long ramtop_val, ramvalid_val;
    
    /* Validate ramvalid first */
    ramvalid_val = Supexec(read_ramvalid);
    if (ramvalid_val < 0 || ramvalid_val != RAMVALID_MAGIC) {
        return 0;
    }
    
    ramtop_val = Supexec(read_ramtop);
    if (ramtop_val < 0) {
        /* Supexec failed */
        return 0;
    }
    
    /* Ensure we don't return negative values due to calculation errors */
    if (ramtop_val <= ALTRAM_START_BOUNDARY) {
        return 0;
    }
    
    return (unsigned long int)(ramtop_val - ALTRAM_START_BOUNDARY);
}

static unsigned long int get_stram_value(void){
    long phystop_val;
    
    phystop_val = Supexec(read_phystop);
    if (phystop_val < 0) {
        /* Supexec failed, return 0 */
        return 0;
    }
    
    return (unsigned long int)phystop_val;
}


static unsigned long int get_available_altram(void){
    long avail;

    if (!is_there_altram()) {
        return 0;
    }
    
    avail = Mxalloc(-1, 1);
    if (avail < 0) {
        /* Mxalloc failed or no Alt-RAM available */
        return 0;
    }

    return (unsigned long int)avail;
}

static unsigned long int get_available_stram(void){
    long avail;

    avail = Mxalloc(-1, 0);
    if (avail < 0) {
        /* Mxalloc failed */
        return 0;
    }

    return (unsigned long int)avail;
}

__typeof__(get_nprocs_conf) __get_nprocs_conf;
int __get_nprocs_conf (void){
    return 1;
}
weak_alias (__get_nprocs_conf, get_nprocs_conf)

/* Return number of available processors.  */
__typeof__(get_nprocs) __get_nprocs;
int __get_nprocs (void){
    return 1;
}
weak_alias (__get_nprocs, get_nprocs)

/* Return number of physical pages of memory in the system.  */
__typeof__(get_phys_pages) __get_phys_pages;
long int __get_phys_pages (void){
    unsigned long int total_mem;
    long page_size;
    
    total_mem = get_stram_value() + get_altram_value();
    if (total_mem == 0) {
        /* No memory detected or system calls failed */
        errno = EIO;
        return -1;
    }
    
    page_size = getpagesize();
    if (page_size <= 0) {
        /* Invalid page size, use safe default */
        page_size = 8192;
    }
    
    return (long int)(total_mem / page_size);
}
weak_alias (__get_phys_pages, get_phys_pages)

/* Return number of available physical pages of memory in the system.  */
__typeof__(get_avphys_pages) __get_avphys_pages;
long int __get_avphys_pages (void){
    unsigned long int avail_mem;
    long page_size;
    
    avail_mem = get_available_stram() + get_available_altram();
    /* Available memory can legitimately be 0 if system is out of memory */
    
    page_size = getpagesize();
    if (page_size <= 0) {
        /* Invalid page size, use safe default */
        page_size = 8192;
    }
    
    return (long int)(avail_mem / page_size);
}
weak_alias (__get_avphys_pages, get_avphys_pages)

/* Return maximum number of processes this real user ID can have.  */
__typeof__(get_child_max) __get_child_max;
long int __get_child_max (void){
#ifdef CHILD_MAX
    return CHILD_MAX;
#else
    return UNLIMITED;
#endif
}
weak_alias (__get_child_max, get_child_max)