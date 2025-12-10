
#ifndef __BRUTALSHELL_WRAPPER_H__
#define __BRUTALSHELL_WRAPPER_H__

#ifndef BUFLEN
#define BUFLEN 256
#endif

#include <termios.h>

#include <unistd.h>

extern struct termios origin;
extern int loglevel;
extern int logfd;

struct config {
	int logfd;
	char **argv;
	int daemon_method;
	void *desc;
	size_t len;
};

struct ring_buffer {
	size_t count;
	size_t buflen;
	char *buf;
};

int connect_daemon( struct config );
int get_session ( int );
ssize_t send_daemon( int method, void *restrict, void *restrict, ssize_t );

int read_pty( void );
int set_pty( void ); /* raw pty */
void reset_pty( void );

void usage( const char *restrict );

struct config get_configure( int, char **restrict );

int set_nonblocking( int );
ssize_t nb_read( void *restrict, void *restrict );
ssize_t nb_write( void *restrict, void *restrict );
ssize_t b_write( int, void *restrict, size_t );
void *dump_ringbuf( void *restrict, size_t, const void *restrict );

enum LOGLEVVELS {
	log_error,
	log_warning,
	log_normal,
	log_debug,
};

#endif
