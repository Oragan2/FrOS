#include "process.h"
#include "mem.h"

struct Process *process[0xffff];
uint32_t LPID = 0;
struct Process *current;

extern void save_context(void); 
extern void switch_context(void);

void newProcess(void func(void)) {
    struct Process *nPro = kmalloc(sizeof(struct Process));
    nPro->pid = ++LPID;
    nPro->state = READY; 
    nPro->kstack = func;

    process[LPID] = nPro;
}

void scheduler(struct Process* new) {
     
}
