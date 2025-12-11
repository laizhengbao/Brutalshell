
#define _XOPEN_SOURCE 600
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>

#ifdef __GNUC__
#	define alloca( size ) __builtin_alloca( size )
#elif defined( __linux__ ) || defined( __sun )
#include <alloca.h>
#elif defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __OpenBSD__ ) || defined( __APPLE__ )
#else
#include <alloca.h>
#endif

#include <signal.h>

#include "wrapper.h"

#include <errno.h>

#ifdef DBG
#include <stdio.h>
#endif

struct termios origin;
int loglevel;
int logfd;

#define loggin( thres, hint, msg, len ) \
	if ( loglevel <= thres ){ \
		write( logfd, hint msg, len ); \
	}

#define logerr( error, len ) \
	loggin( log_error, "Error: ", error, 7 + len )

void winch ( int );
void empty_sigmask( void );
int update_pty( int );

volatile sig_atomic_t need_resieze = 0;

signed main( int argc, char **argv ){

	register int err;

	register int master;

	register char *restrict child_pty_name;

	register int daemon_fd;

	register pid_t pid;

	register int *status;
	register struct sigaction *sa;

	register struct pollfd *fds;
	register struct ring_buffer *bufs;
	register void *buf;
	register ssize_t rlen;

	register struct config cfg = {};

	status = NULL;

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

	daemon_fd = connect_daemon( cfg );

	free( cfg.desc );
	cfg.desc = NULL;

	if ( daemon_fd < 0 ){
		logerr( "Failed to connect the daemon\n", 29 );
		goto ONERR;
	}

	if ( get_session( daemon_fd ) ){
		logerr( "Failed to get session id\n", 21 );
		goto ONERR;
	}


	if ( ( pid = fork() ) < 0 ){
		logerr( "Failed to fork\n", 16 );

		return EXIT_FAILURE;
	}

	if ( !pid ){

		close( master );
		close( daemon_fd );

		if ( setsid() < 0 ){
			logerr( "Failed to setsid\n", 18 );

			return EXIT_FAILURE;
		}

		master = open( child_pty_name, O_RDWR );
		if ( master < 0 ){
			logerr( "Failed to open slave pty\n", 25 );

			return EXIT_FAILURE;
		}

		if ( ioctl( master, TIOCSCTTY, 0 ) < 0 ){
			logerr( "ioctl TIOCSCTTY\n", 16 );
		}

		if ( tcsetpgrp( master, getpid() ) < 0 ){
			logerr( "Failed to tcsetpgrp\n", 20 );
		}

		empty_sigmask();

		dup2( master, STDIN_FILENO );
		dup2( master, STDOUT_FILENO );
		dup2( master, STDERR_FILENO );

		close( master );

		execvp( *cfg.argv, cfg.argv );

		loggin( log_normal, "ERROR: ", "unable to run", 7 + 14 );
		return EXIT_FAILURE;
	}

	if ( cfg.argv && cfg.argv != argv + 1 ){
		err = 0;
		while ( *( cfg.argv + err ) ){
#			ifdef DBG
			fprintf( stderr, "%s\n", *( cfg.argv + err ) );
#			endif
			free( *( cfg.argv + err ) );
			err++;
		}
		free( cfg.argv );
		err = 0;
	}

	cfg.argv = NULL;

	set_pty();

	sa = alloca( sizeof( *sa ) );

	sigemptyset( &( *sa ).sa_mask );
	( *sa ).sa_handler = winch;
	( *sa ).sa_flags = 0;

	if ( sigaction( SIGWINCH, sa, NULL ) < 0 ){
		goto ONERR;
	}

	fds = alloca( sizeof( *fds ) * 3 );
	bufs = alloca( sizeof( *bufs ) * 2 );

	( *( fds + 0 ) ).fd = STDIN_FILENO;
	( *( fds + 1 ) ).fd = master;
	( *( fds + 2 ) ).fd = daemon_fd;

	memset( bufs, 0, sizeof( *bufs ) * 2 );

	( *( bufs + 0 ) ).buf = malloc( BUFLEN );
	( *( bufs + 1 ) ).buf = malloc( BUFLEN );
	( *( bufs + 0 ) ).buflen = BUFLEN;
	( *( bufs + 1 ) ).buflen = BUFLEN;
	buf = malloc( BUFLEN );

	if (
		!( *( bufs + 0 ) ).buf ||
		!( *( bufs + 1 ) ).buf ||
		!buf
	) {
		err = EXIT_FAILURE;
		goto CLEANUP;
	}

	err = 3;
	while ( err-- ){
		( *( fds + err ) ).events = POLLIN;
	}

	while ( 69 ){
		if ( need_resieze ){
			if ( update_pty( master ) ){
				break;
			}
		}

		err = poll( fds, 3, -1 );

		if ( err < 0 ){
			if ( errno == EINTR ){
				continue;
			}

			logerr( "Failed to poll\n", 16 );
			break;
		}

		if ( ( *( fds + 1 ) ).revents & ( POLLHUP | POLLERR | POLLNVAL ) ) {
			err = EXIT_FAILURE;
			break;
		}

		if ( ( *( fds + 2 ) ).revents & ( POLLHUP | POLLERR | POLLNVAL ) ) {
			loggin( log_normal, "Daemon: ", "Disconnected\n", 21 );
			err = EXIT_FAILURE;
			break;
		}

		if (
			( *( fds + 1 ) ).revents & POLLOUT &&
			( err = ( nb_write( fds + 1, bufs + 0 ) < 0 ) )
		){
			break;
		}

		if (
			( *( fds + 2 ) ).revents & POLLOUT &&
			/* give len=0 for ring bufer, else blocking write */
			( err = ( send_daemon( cfg.daemon_method, fds + 2, bufs + 1, 0 ) < 0 ) )
		){
			break;
		}

		if ( ( *( fds + 0 ) ).revents & POLLIN ){
			if ( ( err = ( nb_read( fds + 0, bufs + 0 ) < 0 ) ) ) {
				break;
			}
			( *( fds + 1 ) ).events |= POLLOUT;
		}

		if ( ( *( fds + 1 ) ).revents & POLLIN ){
			if ( ( err = ( rlen = nb_read( fds + 1, bufs + 1 ) ) < 0 ) ) {
				break;
			}
			if ( ( err = ( !dump_ringbuf( buf, BUFLEN, bufs + 1 ) ) ) ) {
				break;
			}
			if ( ( err = ( b_write( STDOUT_FILENO, buf, rlen ) < 0 ) ) ) { /* write back for user */
				break;
			}

			( *( fds + 2 ) ).events |= POLLOUT;
		}

		if ( ( *( fds + 2 ) ).revents & POLLIN ) {
			if ( ( err = ( nb_read( fds + 2, bufs + 0 ) < 0 ) ) ) {
				break;
			}
			( *( fds + 1 ) ).events |= POLLOUT;
		}
	}

CLEANUP:

	close( daemon_fd );

 	free( ( *( bufs + 0 ) ).buf );
 	free( ( *( bufs + 1 ) ).buf );
	free( buf );

	kill( pid, SIGHUP );
	status = alloca( sizeof( *status ) );
	waitpid( pid, status, 0 );

ONERR:

	close( master );

	if ( status ){
		return WEXITSTATUS( *status );
	}

	return err;
}

void winch ( int sig ){
	need_resieze = 1;
}

int update_pty( int fd ){
	struct winsize ws;

#	ifdef DBG
	loggin( log_error, "INFO: ", "SIGWINCH\n", 15 );
#	endif

	if ( ioctl( STDOUT_FILENO, TIOCGWINSZ, &ws ) < 0 ){
		logerr( "ioctl get\n", 10 );
		return -1;
	}
	if ( ioctl( fd, TIOCSWINSZ, &ws ) < 0 ){
		logerr( "ioctl set\n", 10 );
		return -1;
	}

	return 0;
}

void empty_sigmask( void ){

	static sigset_t mask;

	need_resieze = 0;

	sigemptyset( &mask );
	sigprocmask( SIG_SETMASK, &mask, NULL );
	signal( SIGWINCH, SIG_DFL );
}
