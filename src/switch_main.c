
#include <switch.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    while (appletMainLoop()) {
        svcSleepThread(1000000000ULL);
    }

    return 0;
}
