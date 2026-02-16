#include "cmd.h"
#include "string.h"
#include "screen.h"
#define NBCMD 7

// Commands

const char cmd_list[NBCMD][6] = {
    "nty\0",
    "cls\0",
    "ecrit\0",
    "echo\0",
    "aide\0",
    "help\0",
    "prx\0"
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
                case 1: //also cls
                    clear_screen();
                    return;
                case 2: // echo
                case 3:
                    print(s + strlen(cmd)+1);
                    newLine();
                    return;
                case 4: // help
                case 5:
                    print("Commandes disponible:\n");
                    print(" - nettoyer/cls: nettoie l'ecran\n");
                    print(" - ecrit/echo [text]: affiche le texte a l'ecran\n");
                    print(" - aide/help: Afficher ce message\n");
                    return;
                case 6:
                    setPrefix(s+4);
                    return;
                default:
                    return;
            }
        }
        cmdF = 0;
    }
    if (strcmp(s, "\0")) return;
    setColor(0x04);
    print("Commande inconnue : ");
    print(s);
    print("\n");
    setColor(0x0F);
    return;
}
