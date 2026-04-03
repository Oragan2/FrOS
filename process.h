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
    uint32_t rip;
    uint8_t *kstack;
    uint32_t pid;
    State state;
};

#endif
