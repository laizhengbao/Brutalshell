#include <stdio.h>
#include <termios.h>

int test_cleanup_resources(void);

int main(void) {
    int failed = 0;

    printf("Running: test_cleanup_resources...\n");
    if (test_cleanup_resources() != 0) {
        printf("FAILED: test_cleanup_resources\n");
        failed++;
    } else {
        printf("PASSED: test_cleanup_resources\n");
    }
    return 0;
}