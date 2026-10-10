#include <stdint.h>

#include "keyboard.h"
#include "terminal.h"
#include "shell.h"
#include "parser.h"

#define SHELL_BUFFER_SIZE 128

/*
AVALIALBE
*/

static void command_help(int argc, char **argv);
static void command_clear(int argc, char **argv);
static void command_echo(int argc, char **argv);
static void command_about(int argc, char **argv);
static void command_version(int argc, char **argv);

typedef void (*shell_command_fn)(int argc, char **argv);

typedef struct {
    const char *name;
    const char *description;
    shell_command_fn execute;
} shell_builtin_t;


static char shell_buffer[SHELL_BUFFER_SIZE];
static uint32_t shell_length = 0;


/* --------------------------------------------------
 * Utility functions
 * -------------------------------------------------- */

static int shell_streq(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

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



/* --------------------------------------------------
 * Command registry
 * -------------------------------------------------- */

static const shell_builtin_t shell_builtins[] = {
    { "help",    "Show available commands", command_help },
    { "clear",   "Clear the terminal",      command_clear },
    { "echo",    "Print arguments",         command_echo },
    { "about",   "About Keplar",            command_about },
    { "version", "Show version information", command_version },
    {"info", "show info about Keplar", command_about}
};

#define SHELL_BUILTIN_COUNT \
    (sizeof(shell_builtins) / sizeof(shell_builtins[0]))

/* --------------------------------------------------
 * Built-in commands
 * -------------------------------------------------- */

static void command_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    terminal_write("Keplar commands:\n");

    for (uint32_t i = 0; i < SHELL_BUILTIN_COUNT; i++) {
        terminal_write("  ");
        terminal_write(shell_builtins[i].name);
        terminal_write(" - ");
        terminal_write(shell_builtins[i].description);
        terminal_putchar('\n');
    }
}

static void command_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    terminal_clear();
}

static void command_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (i > 1)
            terminal_putchar(' ');

        terminal_write(argv[i]);
    }

    terminal_putchar('\n');
}

static void command_about(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    terminal_write("Keplar OS\n");
    terminal_write("A small x86-64 operating system.\n");
    terminal_write("Named after Johannes Kepler.\n");
    terminal_write("Currently in development.\n");
}

static void command_version(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    keplar_intro();
}


/* --------------------------------------------------
 * Command dispatcher
 * -------------------------------------------------- */

static void shell_dispatch(shell_command_t *command)
{
    for (uint32_t i = 0; i < SHELL_BUILTIN_COUNT; i++) {
        if (shell_streq(
                command->argv[0],
                shell_builtins[i].name)) {

            shell_builtins[i].execute(
                command->argc,
                command->argv
            );

            return;
        }
    }

    terminal_write("Keplar: command not found: ");
    terminal_write(command->argv[0]);
    terminal_putchar('\n');
}


/* --------------------------------------------------
 * Command execution
 * -------------------------------------------------- */

static void shell_execute(void)
{
    shell_command_t command;

    shell_buffer[shell_length] = '\0';

    if (shell_length == 0) {
        terminal_putchar('\n');
        terminal_prompt();
        return;
    }

    terminal_putchar('\n');

    shell_parse_result_t result =
        shell_parse(shell_buffer, &command);

    if (result != SHELL_PARSE_OK) {
        switch (result) {
        case SHELL_PARSE_TOO_MANY_ARGS:
            terminal_write("Keplar: too many arguments.\n");
            break;

        case SHELL_PARSE_UNTERMINATED_QUOTE:
            terminal_write("Keplar: unmatched quote.\n");
            break;

        case SHELL_PARSE_TRAILING_ESCAPE:
            terminal_write("Keplar: trailing escape character.\n");
            break;

        default:
            terminal_write("Keplar: parse error.\n");
            break;
        }

        terminal_prompt();
        return;
    }

    if (command.argc > 0)
        shell_dispatch(&command);

    terminal_prompt();
}


/* --------------------------------------------------
 * Shell lifecycle
 * -------------------------------------------------- */

void shell_init(void)
{
    terminal_clear();
    keplar_intro();
    shell_reset_buffer();
}

void shell_run(void)
{
    terminal_prompt();

    for (;;) {
        char c = keyboard_getchar();

        if (c == 0)
            continue;

        if (c == '\n' || c == '\r') {
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

        shell_buffer[shell_length++] = c;
        shell_buffer[shell_length] = '\0';

        terminal_putchar(c);
    }
}