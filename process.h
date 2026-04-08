#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>


enum State {
    READY,
    RUNNING,
    FINISHED,
    ZOMBIE
};

struct Process {
    uint32_t *esp;
    uint32_t eip;
    uint8_t *kstack;
    uint32_t pid;
    enum State state;
};

void newProcess(void func(void));

#endif
