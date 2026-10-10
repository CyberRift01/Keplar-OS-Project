#include <stdint.h>

#include "pic.h"
#include "io.h"

#define PIC1_COMMAND  0x20
#define PIC1_DATA     0x21

#define PIC2_COMMAND  0xA0
#define PIC2_DATA     0xA1

#define PIC_EOI       0x20

#define ICW1_INIT     0x10
#define ICW1_ICW4     0x01

#define ICW4_8086     0x01

void pic_remap(void) {
    uint8_t master_mask = inb(PIC1_DATA);
    uint8_t slave_mask  = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_unmask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = 0x21;
    } else if (irq < 16) {
        port = 0xA1;
        irq -= 8;
    } else {
        return;
    }

    value = inb(port);
    value &= (uint8_t)~(1u << irq);
    outb(port, value);

    /* If enabling a slave IRQ, also enable the master cascade. */
    if (port == 0xA1) {
        value = inb(0x21);
        value &= (uint8_t)~(1u << 2);
        outb(0x21, value);
    }
}