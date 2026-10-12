#ifndef KEPLAR_TIMER_H
#define KEPLAR_TIMER_H

#include <stdint.h>

void timer_init(void);
uint64_t timer_get_ticks(void);
void ctimer_sleep(uint64_t seconds);
void timer_sleep(uint64_t seconds);

#endif