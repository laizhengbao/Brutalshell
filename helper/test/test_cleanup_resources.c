#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../helper.h"

int test_cleanup_resources(void) {
    struct helper_config cfg = {0};
    struct daemon_response *resp = malloc(sizeof(struct daemon_response));
    if (!resp) {
        return 1; /* test failed */
    }

    cfg.daemon_fd = -1;
    cfg.daemon_path = strdup("/tmp/demo.sock");
    if (!cfg.daemon_path) {
        free(resp);
        return 1; /* test failed */
    }

    cleanup_resources(&cfg, resp);

    if (cfg.daemon_fd != -1 || cfg.daemon_path != NULL) {
        return 1; /* test failed */
    }

    return 0; /* test passed */
}
