
#define _XOPEN_SOURCE 600
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>

#include <alloca.h>
#include <signal.h>

#include "wrapper.h"

struct termios origin;
int loglevel;
int logfd;

#ifndef BUFLEN
#define BUFLEN 1024
#endif

#define loggin( thres, hint, msg, len ) \
	if ( loglevel <= thres ){ \
		write( logfd, hint msg, len ); \
	}

#define logerr( error, len ) \
	loggin( log_error, "Error: ", error, 7 + len )

signed main( int argc, char **argv ){

	register int err;

	register int master;

	register char *restrict child_pty_name;

	register int daemon_fd;
	register char *restrict session_id = NULL;

	register pid_t pid;

	register int *status;

	register struct pollfd *fds;
	register char *buf;
	register ssize_t rlen;

	register struct config cfg = {};

	cfg = get_configure( argc, argv );

	if ( !cfg.argv ){
		return EXIT_FAILURE;
	}

	if ( ( err = read_pty() ) < 0 ){
		logerr( "Failed to get pty attribute\n", 28 );
		return err;
	}

	atexit( reset_pty );

	master = posix_openpt( O_RDWR | O_NOCTTY );

	if ( master < 0 ){
			logerr( "Failed to open master pty\n", 24 );

		return EXIT_FAILURE;
	}

	if ( grantpt( master ) < 0 ){
		logerr( "Failed to grant master fd\n", 24 );

		return EXIT_FAILURE;
	}

	if ( unlockpt( master ) < 0 ){
		logerr( "Failed to unlock pseudo term\n", 27 );

		return EXIT_FAILURE;
	}

	if ( !( child_pty_name = ptsname( master ) ) ){
		logerr( "Failed to get pts name\n", 23 );

		return EXIT_FAILURE;
	}


	if ( ( pid = fork() ) < 0 ){
		logerr( "Failed to fork\n", 16 );

		return EXIT_FAILURE;
	}

	if ( !pid ){
		close( master );

		if ( setsid() < 0 ){
			logerr( "Failed to setsid\n", 18 );

			return EXIT_FAILURE;
		}

		master = open( child_pty_name, O_RDWR );
		if ( master < 0 ){
			logerr( "Failed to open slave pty\n", 25 );

			return EXIT_FAILURE;
		}

		dup2( master, STDIN_FILENO );
		dup2( master, STDOUT_FILENO );
		dup2( master, STDERR_FILENO );

		close( master );

		execvp( *cfg.argv, cfg.argv );

		loggin( log_normal, "ERROR: ", "unable to run", 7 + 14 );
		return EXIT_FAILURE;
	}

	set_pty();

	daemon_fd = connect_daemon( cfg );

	if ( daemon_fd < 0 ){
		logerr( "Failed to connect the daemon\n", 29 );
		kill( pid, SIGTERM );
		waitpid( pid, NULL, 0 );
		goto ONERR;
	}

	session_id = get_session( daemon_fd );

	if ( !session_id ){
		logerr( "Failed to get session id\n", 21 );
		kill( pid, SIGTERM );
		waitpid( pid, NULL, 0 );
		goto ONERR;
	}

	fds = alloca( sizeof( *fds ) * 3 );

	( *( fds + 0 ) ).fd = STDIN_FILENO;
	( *( fds + 0 ) ).events = POLLIN;

	( *( fds + 1 ) ).fd = master;
	( *( fds + 1 ) ).events = POLLIN;

	( *( fds + 2 ) ).fd = daemon_fd;
	( *( fds + 2 ) ).events = POLLIN;

	buf = malloc( BUFLEN );

	while ( 69 ){
		err = poll( fds, 3, -1 );
		if ( err < 0 ){
			logerr( "Failed to poll\n", 16 );
			break;
		}

		if ( ( *( fds + 0 ) ).revents & POLLIN ){
			rlen = read( ( *( fds + 0 ) ).fd, buf, BUFLEN );
			if ( rlen <= 0 ) break;
			write( ( *( fds + 1 ) ).fd, buf, rlen );
		}

		if ( ( *( fds + 1 ) ).revents & POLLHUP ) {
			break;
		}

		if ( ( *( fds + 1 ) ).revents & POLLIN ){
			rlen = read( ( *( fds + 1 ) ).fd, buf, BUFLEN );
			if ( rlen <= 0 ) break;
			write( ( *( fds + 0 ) ).fd, buf, rlen ); /* write back for terminal user */
			send_daemon( cfg.daemon_method, session_id, ( *( fds + 2 ) ).fd, buf, rlen );
		}

		if ( ( *( fds + 2 ) ).revents & POLLIN ){
			rlen = read( ( *( fds + 2 ) ).fd, buf, BUFLEN );
			if ( rlen <= 0 ) break;
			write( ( *( fds + 1 ) ).fd, buf, rlen );
		}
	}

	close( daemon_fd );

	free( session_id );

	free( buf );

ONERR:

	close( master );
	status = alloca( sizeof( *status ) );
	waitpid( pid, status, 0 );

	return WEXITSTATUS( *status );
}
