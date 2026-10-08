#include <stdint.h>
#include "keyboard.h"
#include "terminal.h"
#include "shell.h"

#define SHELL_BUFFER_SIZE 128

static char shell_buffer[SHELL_BUFFER_SIZE];
static uint32_t shell_length = 0;

static void shell_reset_buffer(void)
{
    shell_length = 0;
    shell_buffer[0] = '\0';
}

static void shell_backspace(void)
{
    if (shell_length == 0)
        return;

    shell_length--;

    shell_buffer[shell_length] = '\0';

    terminal_putchar('\b');
}

static void shell_execute(void)
{
    shell_buffer[shell_length] = '\0';

    if (shell_length == 0) {
        terminal_putchar('\n');
        terminal_prompt();
        return;
    }

    if (shell_length == 4 &&
        shell_buffer[0] == 'h' &&
        shell_buffer[1] == 'e' &&
        shell_buffer[2] == 'l' &&
        shell_buffer[3] == 'p') {

        terminal_putchar('\n');

        terminal_write("Keplar commands:\n");
        terminal_write("  help     - show available commands\n");
        terminal_write("  clear    - clear the terminal\n");
        terminal_write("  echo     - print text\n");
        terminal_write("  about    - show information about Keplar\n");
        terminal_write("  version  - show the Keplar version\n");

        terminal_prompt();
        return;
    }

    if (shell_length == 5 &&
        shell_buffer[0] == 'c' &&
        shell_buffer[1] == 'l' &&
        shell_buffer[2] == 'e' &&
        shell_buffer[3] == 'a' &&
        shell_buffer[4] == 'r') {

        terminal_clear();
        terminal_prompt();
        return;
    }

    if (shell_length == 5 &&
        shell_buffer[0] == 'a' &&
        shell_buffer[1] == 'b' &&
        shell_buffer[2] == 'o' &&
        shell_buffer[3] == 'u' &&
        shell_buffer[4] == 't') {

        terminal_putchar('\n');

        terminal_write("Keplar OS\n");
        terminal_write("A small x86-64 operating system.\n");
        terminal_write("Named after Johannes Kepler.\n");
        terminal_write("Currently in development.\n");

        terminal_prompt();
        return;
    }

    if (shell_length == 7 &&
        shell_buffer[0] == 'v' &&
        shell_buffer[1] == 'e' &&
        shell_buffer[2] == 'r' &&
        shell_buffer[3] == 's' &&
        shell_buffer[4] == 'i' &&
        shell_buffer[5] == 'o' &&
        shell_buffer[6] == 'n') {

        terminal_putchar('\n');

        keplar_intro();

        terminal_prompt();
        return;
    }

    if (shell_length >= 5 &&
        shell_buffer[0] == 'e' &&
        shell_buffer[1] == 'c' &&
        shell_buffer[2] == 'h' &&
        shell_buffer[3] == 'o' &&
        shell_buffer[4] == ' ') {

        terminal_putchar('\n');

        for (uint32_t i = 5; i < shell_length; i++)
            terminal_putchar(shell_buffer[i]);

        terminal_putchar('\n');

        terminal_prompt();
        return;
    }

    terminal_putchar('\n');

    terminal_write("Keplar: command not found!");
    terminal_write(shell_buffer);
    terminal_putchar('\n');

    terminal_prompt();
}

void shell_init(void) {
    terminal_clear();
    keplar_intro();
    shell_reset_buffer();
}

void shell_run(void) {
    terminal_prompt();

    for (;;) {
        char c = keyboard_getchar();

        if (c == 0)
            continue;

        if (c == '\n') {
            shell_execute();
            shell_reset_buffer();
            continue;
        }

        if (c == '\b') {
            shell_backspace();
            continue;
        }

        if (c < 32)
            continue;

        if (shell_length >= SHELL_BUFFER_SIZE - 1)
            continue;

        shell_buffer[shell_length] = c;
        shell_length++;

        terminal_putchar(c);
        
    }
}