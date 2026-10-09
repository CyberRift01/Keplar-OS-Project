#include <stdint.h>
#include "pic.h"

volatile uint64_t timer_ticks = 0;

void irq0_handler(void) {
    timer_ticks++;
    pic_send_eoi(0);
}

void irq1_handler(void) {
    pic_send_eoi(1);
}
