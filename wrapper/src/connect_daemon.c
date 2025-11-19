
#define _POSIC_C_SOURCE 200809L
#include "wrapper.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/un.h>

int connect_daemon( struct config cfg ){

	register int dfd = -1;

	union {
		struct sockaddr addr;
		struct sockaddr_un uaddr;
	} addr;

	switch( cfg.daemon_method ){
		case 0:
			dfd = socket( AF_UNIX, SOCK_STREAM, 0 );
			if ( dfd < 0 ){
				break;
			}
			memset( &addr.uaddr, 0, sizeof( addr.uaddr ) );

			addr.uaddr.sun_family = AF_UNIX;
			strncpy( addr.uaddr.sun_path, cfg.desc, sizeof( addr.uaddr.sun_path ) - 1 );

			if ( connect( dfd, &addr.addr, sizeof( addr.uaddr ) ) < 0 ){
				close( dfd );
			}

			break;
	};

	return dfd;
}
