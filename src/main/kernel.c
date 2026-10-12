 #include <stdint.h>


/*
 * All Local files should be included here
*/
#include "keyboard.h"
#include "terminal.h"
#include "shell.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "status.h"


/*
 * All defination should be here
*/

/*
 * Global Varibale
*/


void kernel_main(void)
{
    __asm__ volatile ("cli" ::: "memory");

    keplar_intro();

    pic_remap();
    idt_init();
    idt_set_irq_gates();

    keyboard_init();
    timer_init();
    pic_unmask_irq(1);

    shell_init();

    __asm__ volatile ("sti" ::: "memory");

    shell_run();

    for (;;) {
        __asm__ volatile ("cli; hlt" ::: "memory");
    }
}