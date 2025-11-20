
#include "wrapper.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef SESSION_ID_LEN
#define SESSION_ID_LEN 32
#endif

char *restrict get_session( int fd ){

	register char *restrict ret = NULL;
	static const char *restrict request = "NEW_SESSION_ID";

	if ( fd < 0 ){
		return ret;
	}

	if ( write( fd, request, strlen( request ) ) < 0 ){
		return ret;
	}

	ret = malloc( SESSION_ID_LEN + 1 );
	memset( ret, 0, SESSION_ID_LEN );

	if ( read( fd, ret, SESSION_ID_LEN ) != SESSION_ID_LEN ){
		free( ret );
		return NULL;
	}

	setenv( "BSH_TERM_SESSION", ret, 1 );

	return ret;
}
