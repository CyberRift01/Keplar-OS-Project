#include <stdint.h>

#include "terminal.h"

void status_function(uint8_t status) {
    if (status) {
        terminal_backward_put_char(']',1);
        terminal_backward_put_char(' ',2);
        terminal_backward_put_char(' ',3);
        terminal_backward_put_char(' ',4);
        cterminal_backward_put_char('K',5,0x02);
        cterminal_backward_put_char('O',6,0x02);
        terminal_backward_put_char(' ', 7);
        terminal_backward_put_char(' ', 8);
        terminal_backward_put_char(' ', 9);
        terminal_backward_put_char('[', 10);
        terminal_putchar('\n');

    }
    else
    {
        terminal_write("[ ");
        cterminal_write("Failed", 0x04);
        terminal_write(" ]");
    }
}
