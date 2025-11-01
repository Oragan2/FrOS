#include "cmd.h"
#include "string.h"
#include "screen.h"
#define NBCMD 3

// Commands

const char cmd_list[NBCMD][6] = {
    "cls\0",
    "echo\0",
    "help\0"
};

// End Commands

void cmdCheck(const char *s) {
    char cmd[20];
    cmd[0] = '\0';
    int cmdF = 0;
    size_t pos = strpos(s, 32); // ASCII space is 32
    if (pos != -1) {
        strcut(s, pos-1, 1, cmd);
    }
    for (int i = 0; i < NBCMD; ++i) {
        if (cmd[0] == '\0')
            cmdF = strcmp(s, cmd_list[i]);
        else {
            cmdF = strcmp(cmd, cmd_list[i]);
        }
        if (cmdF) {
            switch (i) {
                case 0: // cls
                    clear_screen();
                    return;
                case 1: // echo
                    print(s + 5); // 5 is length of "echo "
                    newLine();
                    return;
                case 2: // help
                    print("Available commands:\n");
                    print(" - cls: Clear the screen\n");
                    print(" - echo [text]: Print text to the screen\n");
                    print(" - help: Show this help message\n");
                    return;
                default:
                    return;
            }
        }
        cmdF = 0;
    }
    return;
}
