#define _POSIX_C_SOURCE 200809L

#include "../helper.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Send message to daemon.
 * Returns 0 on success, -1 on failure.
 */
int send_to_daemon(int fd, const char *data, ssize_t len) {
  ssize_t written = 0;
  ssize_t total = (len < 0) ? (ssize_t)strlen(data) : len;

  if (fd < 0) {
    print_error("invalid file descriptor");
    return -1;
  }

  if (!data) {
    print_error("data is NULL");
    return -1;
  }

  /* Write data to socket */
  written = write(fd, data, total);
  if (written < 0) {
    fprintf(stderr, "write error: %s\n", strerror(errno));
    return -1;
  }

  if (written != total) {
    fprintf(stderr, "write incomplete: %ld/%ld bytes\n", written, total);
    return -1;
  }

  return 0;
}
