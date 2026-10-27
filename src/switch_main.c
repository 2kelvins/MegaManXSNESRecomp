
#include <switch.h>
#include <stdio.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    consoleInit(NULL);

    printf("MEGA MAN X - SWITCH\n");
    printf("===================\n\n");
    printf("TESTE 13\n");
    printf("Console inicializado!\n");

    while (appletMainLoop()) {
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
