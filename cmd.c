#include "cmd.h"
#include "string.h"
#include "screen.h"
#include "fs.h"
#include "pci.h"
#include "mem.h"

#define NBCMD 13 

// Commands

const char cmd_list[NBCMD][7] = {
    "nty\0",
    "cls\0",
    "ecrit\0",
    "echo\0",
    "aide\0",
    "help\0",
    "ls\0",
    "cat\0",
    "aff\0",
    "lspci\0",
    "lsmem\0",
    "touch\0",
    "nvf\0",
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
        if (!cmdF) {
            switch (i) {
                case 0: // cls
                case 1: //also cls
                    clear_screen();
                    return;
                case 2: // echo
                case 3:
		    int a = strpos(s+strlen(cmd)+1, '>'); 
		    if (a == -1) {
                    	print(s + strlen(cmd)+1);
                    	newLine();
		    }
		    else {
			const char *filename = s+strlen(cmd)+2+a;
			print(filename);
			print("\n");
			char* text;
			strcut(s+strlen(cmd)+1,a-1,1,text);
			struct FILE* f = findFile(filename);
			if (f == NULL) {
				setColor(0x04);
				print("File '");
				print(filename);
				print("' not found\n");
				setColor(0x0F);
				return;
			}
			writeFile(f,text);
		    }
                    return;
                case 4: // help
                case 5:
                    print("Commandes disponible:\n");
                    print(" - nettoyer/cls: nettoie l'ecran\n");
                    print(" - ecrit/echo [text]: affiche le texte a l'ecran\n");

                    print(" - aide/help: Afficher ce message\n");
                    return;
                case 6: // ls
                    for (int i = 0; i < filesys.nbFiles; ++i) {
			print(filesys.root[i].name);
			print("\n");
		    }
                    return;
		case 7: // cat
		case 8:
		    struct FILE* f = findFile(s+strlen(cmd)+1);
		    if (f != NULL) 
		    {
			    char buffer[f->size*512];
			    readFile(f, buffer);
			    if (*buffer)
			    	print(buffer);
			    return;
		    }
		    setColor(0x04);
		    print("File not found\n");
		    setColor(0x0F);
		    return;
            case 9: // lspci
                pci_scan();
                return;
            case 10: // memcheck
                if (memcheck((void*)0x80, 0x100000)) {
                    setColor(0x04);
                    print("Memory check failed\n");
                    setColor(0x0F);
                } else {
                    setColor(0x02);
                    print("Memory check passed\n");
                    setColor(0x0F);
                }
                return;
		    case 11: // touch
		    case 12:
		        saveFile(s+strlen(cmd)+1);
                return;
        default:
            return;
            }
        }
        cmdF = 0;
    }
    if (!strcmp(s, "\0")) return;
    setColor(0x04);
    print("Commande inconnue : ");
    print(s);
    print("\n");
    setColor(0x0F);
    return;
}
