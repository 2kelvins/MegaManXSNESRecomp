
#include <switch.h>
#include <stdio.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    consoleInit(NULL);

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    PadState pad;
    padInitializeDefault(&pad);

    printf("MEGA MAN X - NINTENDO SWITCH\n");
    printf("===========================\n\n");
    printf("TESTE 14 - CONTROLES\n\n");
    printf("Pressione A, B ou X.\n");
    printf("Pressione + para sair.\n\n");

    while (appletMainLoop()) {
        padUpdate(&pad);

        u64 buttons = padGetButtonsDown(&pad);

        if (buttons & HidNpadButton_Plus) {
            break;
        }

        if (buttons & HidNpadButton_A) {
            printf("Botao A pressionado!\n");
        }

        if (buttons & HidNpadButton_B) {
            printf("Botao B pressionado!\n");
        }

        if (buttons & HidNpadButton_X) {
            printf("Botao X pressionado!\n");
        }

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
