
#include "wrapper.h"

#include <string.h>

#ifndef _min
#define _min(a, b)		\
	((a) < (b)) ? (a) : (b)
#endif

void *dump_ringbuf( void *restrict dest, size_t buflen, const void *restrict src ){

	register void *ret = NULL;

	register size_t dlen = _min( ( *(struct ring_buffer *)src ).count, buflen );

	ret = memcpy( dest, ( *(struct ring_buffer *)src ).buf, dlen );
	(void)( ret && ( ret += dlen ) );

	return ret;
}
