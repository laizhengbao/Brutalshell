
#include "wrapper.h"

#include <fcntl.h>

int set_nonblocking( int fd ){
	register int flags;

	flags = fcntl( fd, F_GETFL, 0 );

	if ( flags < 0 ){
		return flags;
	}

	return fcntl( fd, F_SETFL, flags | O_NONBLOCK );
}
