#ifndef __BRUTALSHELL_HELPER_H__
#define __BRUTALSHELL_HELPER_H__

#include <sys/types.h>

struct helper_config {
  int daemon_fd;             /* socket file descriptor to daemon */
  char *daemon_path;         /* UNIX socket path for daemon */
  char helper_session[256];  /* helper session ID (PID-based or UUID) */
  char wrapper_session[256]; /* wrapper session ID received from wrapper */
  char **argv;               /* command-line arguments */
  int argc;                  /* argument count */
};

enum LOG_LEVELS {
  LOG_ERROR = 0,
  LOG_WARNING = 1,
  LOG_INFO = 2,
  LOG_DEBUG = 3,
};

extern int loglevel;
extern int logfd;

int connect_daemon(struct helper_config *cfg);
int get_session(int fd, char *session_id, size_t len);
int send_to_daemon(int fd, const char *data, ssize_t len);
struct daemon_response *recv_from_daemon(int fd);
int confirm_execution(const char *code);
void cleanup_resources(struct helper_config *cfg, struct daemon_response *resp);
void print_error(const char *msg);

#endif
