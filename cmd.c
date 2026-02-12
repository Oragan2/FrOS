#include "cmd.h"
#include "string.h"
#include "screen.h"
#define NBCMD 4

// Commands

const char cmd_list[NBCMD][6] = {
    "nty\0",
    "ecrit\0",
    "aide\0",
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
                    clear_screen();
                    return;
                case 1: // echo
                    print(s + 6); // 5 is length of "echo "
                    newLine();
                    return;
                case 2: // help
                    print("Commandes disponible:\n");
                    print(" - nettoyer: nettoie l'ecran\n");
                    print(" - ecrit [text]: affiche le texte a l'ecran\n");
                    print(" - aide: Afficher ce message\n");
                    return;
                case 3:
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
