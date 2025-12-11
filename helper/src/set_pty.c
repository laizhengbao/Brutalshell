#define _POSIX_C_SOURCE 200809L

#include "../helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void cleanup_resources(struct helper_config *cfg,
                       struct daemon_response *resp) {
  /* Close daemon socket */
  if (cfg && cfg->daemon_fd >= 0) {
    close(cfg->daemon_fd);
    cfg->daemon_fd = -1;
  }

  if (resp) {
    // Free daemon response structure
    free(resp);
  }

  if (cfg) {
    if (cfg->daemon_path) {
      free(cfg->daemon_path);
      cfg->daemon_path = NULL;
    }
    if (cfg->argv) {
      cfg->argv = NULL;
    }
  }
}

void print_error(const char *msg) {
  if (!msg) {
    return;
  }

  fprintf(stderr, "Error: %s\n", msg);
  fflush(stderr);
}
