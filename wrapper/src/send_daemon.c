
#include "wrapper.h"

#include <unistd.h>

#include <string.h>
#include <stdlib.h>

ssize_t send_daemon ( int method, void *restrict fd, void *restrict content, ssize_t len ){

	register ssize_t ret = -1;

	switch ( method ){
		case 0:
			if ( len ){
				ret = b_write( *(int *)fd, content, len );
			} else {
				ret = nb_write( fd, content );
			}

			break;
	}

	return ret;
}
