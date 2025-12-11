#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../helper.h"

int connect_daemon(struct helper_config *cfg) {
  int fd = -1;
  struct sockaddr_un addr;
  const char *socket_path = cfg->daemon_path;

  if (!socket_path) {
    print_error("daemon_path not set");
    return -1;
  }

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
    print_error("socket creation failed");
    return -1;
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
    fprintf(stderr, "connect error: %s\n", strerror(errno));
    close(fd);
    return -1;
  }

  cfg->daemon_fd = fd;
  return fd;
}
