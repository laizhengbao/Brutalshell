
#include "wrapper.h"

#include <errno.h>

ssize_t b_write( int fd, void *restrict buf, size_t len ){

	register ssize_t sent;
	register ssize_t got;

	sent = 0;

	while ( sent < len ){
		got = write( fd, buf + sent, len - sent );
		if ( got > 0 ){
			sent += got;
		} else if ( got < 0 && errno != EINTR ){
			return got;
		}
	}

	return sent;
}
