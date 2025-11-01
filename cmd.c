#include "cmd.h"
#include "string.h"
#include "screen.h"

// Commands

const char cmd_list[2][6] = {
    "cls\0",
    "echo\0"
};

// End Commands

void cmdCheck(const char *s) {
    char* cmd = NULL;
    int cmdF = 0;
    size_t pos = strpos(s, 32); // ASCII space is 32
    if (pos != -1) {
        cmd = strcut(s, pos, 1);
        print(cmd);
    }
    for (int i = 0; i < 2; ++i) {
        if (cmd == NULL) 
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
                    print("Echo: ");
                    print(s + 5);
                    return;
                default:
                    return;
            }
        }
        cmdF = 0;
    }
    return;
}
