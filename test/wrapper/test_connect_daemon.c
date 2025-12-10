#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "wrapper.h"

/* Global stubs declared in main.c */
extern struct termios origin;
extern int loglevel;
extern int logfd;

int test_connect_daemon(void) {
    char tmpl[] = "bsh_no_socketXXXXXX";
    int tmpfd = mkstemp(tmpl);
    if (tmpfd < 0) {
        return 1; /* skip if mkstemp unavailable */
    }
    close(tmpfd); /* leave a regular file to force ENOTSOCK on connect */

    int fd = connect_daemon((struct config){.daemon_method = 0, .desc = tmpl});
    unlink(tmpl);

    if (fd != -1) {
        close(fd);
        return 1; /* test failed */
    }
    return 0; /* test passed */
}
