#include <stdint.h>

#include "io.h"
#include "pic.h"
#include "timer.h"

extern volatile uint64_t timer_ticks;

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_BASE_FREQUENCY 1193182u
#define PIT_FREQUENCY 100u

void timer_init(void)
{
    uint16_t divisor =
        (uint16_t)(PIT_BASE_FREQUENCY / PIT_FREQUENCY);

    /* Channel 0, low byte then high byte, square-wave mode. */
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)(divisor >> 8));

    /* Allow timer IRQ0 through the PIC. */
    pic_unmask_irq(0);
}

uint64_t timer_get_ticks(void)
{
    return timer_ticks;
}