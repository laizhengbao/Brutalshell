
#include "wrapper.h"

#include <sys/poll.h>
#include <string.h>

#include <errno.h>

#ifndef _min
#define _min(a, b) ( (a) < (b) ) ? (a) : (b)
#endif

ssize_t nb_read( void *restrict fd, void *restrict buf ){

	register ssize_t got;
	register size_t slen;

	char tmpbuf[BUFLEN];

	slen = ( *(struct ring_buffer *)buf ).buflen - ( *(struct ring_buffer *)buf ).count;
	slen = _min( slen, BUFLEN );

	got = read( ( *(struct pollfd *)fd ).fd, tmpbuf, slen );

	if ( got < 0  ){
		if ( errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ){
			got = 0;
		}
	} else {
		(void)(
			got &&
			memcpy(
				( *(struct ring_buffer *)buf ).buf +
				( *(struct ring_buffer *)buf ).count,
				tmpbuf,
				got
			) &&
			( ( *(struct ring_buffer *)buf ).count += got )
		);
	}

	return got;
}
