#include <stdint.h>
#include "idt.h"

// All external functions should be placed here

extern void exception_handler(void);

// End of external

// All defination should be placed here

#define IDT_ENTRIES 256

// End of defination

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idt_ptr;

static void idt_set_gate( uint8_t vector,uint64_t handler, uint16_t selector, uint8_t type_attr) {

    idt[vector].offset_low =
        (uint16_t)(handler & 0xFFFF);

    idt[vector].selector = selector;

    idt[vector].ist = 0;

    idt[vector].type_attr = type_attr;

    idt[vector].offset_mid =
        (uint16_t)((handler >> 16) & 0xFFFF);

    idt[vector].offset_high =
        (uint32_t)((handler >> 32) & 0xFFFFFFFF);

    idt[vector].zero = 0;
}

void idt_init(void) {
    for (uint16_t i = 0; i < IDT_ENTRIES; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].type_attr = 0;
        idt[i].offset_mid = 0;
        idt[i].offset_high = 0;
        idt[i].zero = 0;
    }

    /* Install a basic fatal handler for CPU exceptions. */
    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(i, (uint64_t)exception_handler, 0x08, 0x8E);
    }

    /* Install timer and keyboard IRQ gates. */
    

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)idt;

    idt_set_irq_gates();

    /*
     * Load the IDT.
     */
    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_ptr)
    );
}
void idt_set_irq_gates(void)
{
    extern void irq0_stub(void);

    idt_set_gate(0x20, (uint64_t)irq0_stub, 0x08, 0x8E);
}