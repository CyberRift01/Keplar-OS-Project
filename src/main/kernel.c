#include "terminal.h"
#include "shell.h"
#include "idt.h"
#include "pic.h"

void kernel_main(void) {
    idt_init();
    pic_remap();

    keplar_intro();

    shell_init();
    shell_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
