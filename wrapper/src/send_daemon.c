
#include "wrapper.h"

#include <unistd.h>

#include <string.h>
#include <stdlib.h>

int send_daemon ( int method, int fd, char *restrict content, ssize_t len ){

	register int ret = -1;

	switch ( method ){
		case 0:
			ret = ( write( fd, content, len ) != len );

			break;
	}

	return ret;
}
