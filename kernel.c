// kernel.c
#include "string.h"
#include "cmd.h"
#include "io.h"

void kernel_main(void) {
    clear_screen();
    print("Hello, World from Kernel!\n");
    setPrefix("> ");
    print("Started listening for keyboard input...\n");
    print("");
    while (1) {
        keyboard_handler();
    }
}
