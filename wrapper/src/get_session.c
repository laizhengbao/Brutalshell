
#include "wrapper.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef SESSION_ID_LEN
#define SESSION_ID_LEN 32
#endif

int get_session( int fd ){

	register int ret = -1;
	static const char *restrict request = "NEW_SESSION_ID";
	register char *restrict buf;

	if ( fd < 0 ){
		return ret;
	}

	if ( write( fd, request, strlen( request ) ) < 0 ){
		return ret;
	}

	buf = malloc( SESSION_ID_LEN + 1 );
	memset( buf, 0, SESSION_ID_LEN );

	if ( read( fd, buf, SESSION_ID_LEN ) != SESSION_ID_LEN ){
		free( buf );
		return ret;
	}

	ret = setenv( "BSH_TERM_SESSION", buf, 1 );

	free( buf );

	return ret;
}
