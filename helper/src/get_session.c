#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../helper.h"

#ifndef SESSION_ID_LEN
#define SESSION_ID_LEN 32
#endif

int get_session(int fd, char *session_id, size_t len) {
  ssize_t bytes_read;
  static const char *restrict request = "NEW_SESSION_ID";

  if (fd < 0) {
    print_error("invalid file descriptor");
    return -1;
  }

  if (!session_id || len < SESSION_ID_LEN) {
    print_error("session_id buffer too small");
    return -1;
  }

  if (write(fd, request, strlen(request)) < 0) {
    print_error("failed to send NEW_SESSION_ID request");
    return -1;
  }

  bytes_read = read(fd, session_id, SESSION_ID_LEN);
  if (bytes_read != SESSION_ID_LEN) {
    print_error("failed to read session ID from daemon");
    return -1;
  }
  if (len > SESSION_ID_LEN) {
    session_id[SESSION_ID_LEN] = '\0';
  } else {
    session_id[len - 1] = '\0';
  }

  if (getenv("BSH_HELPER_SESSION") == NULL) {
    int setres = setenv("BSH_HELPER_SESSION", session_id, 1);
    if (setres != 0) {
      print_error("failed to set BSH_HELPER_SESSION");
      return -1;
    }
  }

  return 0;
}
