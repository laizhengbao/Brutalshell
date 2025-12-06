
#ifndef __BRUTALSHELL_WRAPPER_H__
#define __BRUTALSHELL_WRAPPER_H__

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

int connect_daemon( struct config );
int get_session ( int );
int send_daemon( int method, int fd, char *restrict, ssize_t );

int read_pty( void );
int set_pty( void ); /* raw pty */
void reset_pty( void );

void usage( const char *restrict );

struct config get_configure( int, char **restrict );

enum LOGLEVVELS {
	log_error,
	log_warning,
	log_normal,
	log_debug,
};

#endif
