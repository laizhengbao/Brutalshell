
#include "wrapper.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void usage ( const char *restrict prog ){
	const static char *const use =
		" [? | c <file> | program]\n\
		?\t\t\tShow this message\n\
		c\t\t\tConfig file to override\n\
		program\t\t\tThe program to execute, default /bin/bash\n\
		";
	write( STDERR_FILENO, "Usage: ", 7 );
	write( STDERR_FILENO, prog, strlen( prog ) );
	write( STDERR_FILENO, use, strlen( use ) );
}
