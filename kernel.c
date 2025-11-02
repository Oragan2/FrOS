// kernel.c
#include "string.h"
#include "cmd.h"
#include "io.h"

void kernel_main(void) {
    char toPrint[2];
    toPrint[1] = '\0';
    clear_screen();
    print("Hello, World from Kernel!\n");
    print("Started listening for keyboard input...\n");
    setPrefix("> ");
    while (1) {
        keyboard_handler();
    }
}
