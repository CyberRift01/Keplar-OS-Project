#ifndef KEPLAR_TIMER_H
#define KEPLAR_TIMER_H

#include <stdint.h>

void timer_init(void);
uint64_t timer_get_ticks(void);
uint8_t ctimer_sleep(uint64_t seconds);
uint8_t timer_sleep(uint64_t seconds);
uint64_t timer_get_seconds(void);

#endif