#include <stdio.h>
#include <termios.h>

/* Global stubs that production code expects. */
struct termios origin = {0};
int loglevel = 0;
int logfd = 2; /* default to stderr */

/* Forward declare test functions */
int test_connect_daemon(void);
int test_get_configure(void);

int main(void) {
    int failed = 0;

    printf("Running: test_connect_daemon...\n");
    if (test_connect_daemon() != 0) {
        printf("FAILED: test_connect_daemon\n");
        failed++;
    } else {
        printf("PASSED: test_connect_daemon\n");
    }

    printf("Running: test_get_configure...\n");
    if (test_get_configure() != 0) {
        printf("FAILED: test_get_configure\n");
        failed++;
    } else {
        printf("PASSED: test_get_configure\n");
    }

    printf("\n%s\n", failed == 0 ? "All tests passed!" : "Some tests failed!");
    return failed;
}
