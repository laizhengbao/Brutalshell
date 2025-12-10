
#include "wrapper.h"

#include <string.h>
#include <sys/poll.h>

#include <errno.h>

ssize_t nb_write( void *restrict fd, void *restrict buf ){
	
	register ssize_t sent;

	sent = 0;

	if ( !( *(struct ring_buffer *)buf ).count ){
		( *(struct pollfd *)fd ).events &= ~POLLOUT;
		return sent;
	}

	sent = write(
			( *(struct pollfd *)fd ).fd,
			( *(struct ring_buffer *)buf ).buf,
			( *(struct ring_buffer *)buf ).count
		);

	if ( sent < 0 ){
		if ( errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ){
			sent = 0;
		}
	} else {
		(void)(
			(
				( ( *(struct ring_buffer *)buf ).count -= sent ) ||
				( ( ( *(struct pollfd *)fd ).events &= ~POLLOUT ) && 0 )
			) &&
			memmove(
				( *(struct ring_buffer *)buf ).buf,
				( *(struct ring_buffer *)buf ).buf + sent,
				( *(struct ring_buffer *)buf ).count
			)
		);
	}

	return sent;
}
