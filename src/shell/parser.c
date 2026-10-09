#include "parser.h"

static int is_whitespace(char c)
{
    return c == ' ' || c == '\t';
}

shell_parse_result_t shell_parse(
    char *input,
    shell_command_t *command)
{
    char *read = input;
    char *write = input;

    command->argc = 0;

    while (*read) {
        /* Skip whitespace between arguments. */
        while (is_whitespace(*read))
            read++;

        if (*read == '\0')
            break;

        if (command->argc >= SHELL_MAX_ARGS)
            return SHELL_PARSE_TOO_MANY_ARGS;

        command->argv[command->argc++] = write;

        char quote = '\0';

        while (*read) {
            /* Whitespace ends an unquoted argument. */
            if (quote == '\0' && is_whitespace(*read))
                break;

            /* Begin a quoted section. */
            if (quote == '\0' &&
                (*read == '"' || *read == '\'')) {
                quote = *read++;
                continue;
            }

            /* End the current quoted section. */
            if (quote != '\0' && *read == quote) {
                quote = '\0';
                read++;
                continue;
            }

            /*
             * Backslash escapes the next character,
             * except inside single quotes.
             */
            if (*read == '\\' && quote != '\'') {
                read++;

                if (*read == '\0')
                    return SHELL_PARSE_TRAILING_ESCAPE;

                *write++ = *read++;
                continue;
            }

            *write++ = *read++;
        }

        if (quote != '\0')
            return SHELL_PARSE_UNTERMINATED_QUOTE;

        /*
         * Advance past separators before inserting NUL,
         * so we don't overwrite unread input.
         */
        while (is_whitespace(*read))
            read++;

        *write++ = '\0';
    }

    return SHELL_PARSE_OK;
}