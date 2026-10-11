 #include <stdint.h>


/*
 * All Local files should be included here
*/
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

void kernel_main(void) {
    terminal_write("Welcome to Keplar!\n");
    terminal_write("Keplar is going to boot\n");
    keplar_intro();

    pic_remap();
    timer_init();
    idt_init();
    
    shell_init();
    shell_run();
    
}
