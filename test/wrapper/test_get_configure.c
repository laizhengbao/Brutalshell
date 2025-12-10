#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "wrapper.h"

/* Global stubs declared in main.c */
extern struct termios origin;
extern int loglevel;
extern int logfd;

static void free_config(struct config cfg) {
    if (cfg.desc) {
        free(cfg.desc);
    }
    if (cfg.argv) {
        for (size_t i = 0; cfg.argv[i]; ++i) {
            free(cfg.argv[i]);
        }
        free(cfg.argv);
    }
}

int test_get_configure(void) {
    /* '?' should early-return with argv == NULL. */
    struct config help_cfg = get_configure(2, (char *[]){"brutalshell", "?"});
    if (help_cfg.argv != NULL) {
        return 1; /* test failed */
    }

    /* Prepare a temp YAML config file. */
    char tmpl[] = "bsh_cfgXXXXXX";
    int fd = mkstemp(tmpl);
    if (fd < 0) {
        return 1; /* skip if mkstemp unavailable */
    }

    static const char *restrict const yaml =
        "method: 1\n"
        "path: /tmp/demo.sock\n"
        "shell:\n"
        "  - /bin/zsh\n"
        "  - -c\n"
        "  - echo hi\n";
    ssize_t written = write(fd, yaml, strlen(yaml));
    if (written != (ssize_t)strlen(yaml)) {
        close(fd);
        unlink(tmpl);
        return 1;
    }
    close(fd);

    struct config cfg = get_configure(3, (char *[]){"brutalshell", "c", tmpl});

    int result = 0;
    if (cfg.daemon_method != 1) {
        result = 1;
    } else if (!cfg.desc || strcmp(cfg.desc, "/tmp/demo.sock") != 0) {
        result = 1;
    } else if (!cfg.argv) {
        result = 1;
    }

    free_config(cfg);
    unlink(tmpl);

    return result;
}
