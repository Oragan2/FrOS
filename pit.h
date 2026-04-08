#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void pit_init();
void time_handler(); //Not finished do not TOUCH
uint32_t pit_get_tick();

#endif
