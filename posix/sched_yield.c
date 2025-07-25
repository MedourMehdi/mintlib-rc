/*
 * sched_yield implemented by Alan Hourihane <alanh@fairlite.co.uk>
 */
#include <mint/mintbind.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>

int 
sched_yield (void)
{
	int y;

	/* Check if we're in a multithreaded environment and not the main thread */
	if ( __mint_is_multithreaded && pthread_self() > 0) {
		/* Use pthread yield for multithreaded environment */
		return pthread_yield();
	}

	y = Syield();
	if (y < 0) {
		__set_errno(-y);
		return -1;
	}

	return 0;
}
