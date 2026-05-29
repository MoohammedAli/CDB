#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "repl/repl.h"
#include "table/table.h"
#include "statement/statement.h"

typedef enum {META_COMMAND_SUCCESS, META_COMMAND_UNRECOGNIZED_COMMAND} MetaCommandResult;


MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

int main(int argc, char* argv[]) {
    if (argc < 2) { printf("Must supply a database filename\n"); exit(EXIT_FAILURE); }

    Table* table = db_open(argv[1]);
    InputBuffer* input_buffer = new_input_buffer();

    while (1) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer, table)) {
                case META_COMMAND_SUCCESS: continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS: break;
            case PREPARE_STRING_TOO_LONG: printf("String is too long.\n"); continue;
            case PREPARE_SYNTAX_ERROR: printf("Syntax error.\n"); continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'\n", input_buffer->buffer);
                continue;
        }

        switch (execute_statement(&statement, table)) {
            case EXECUTE_SUCCESS: printf("Executed.\n"); break;
            case EXECUTE_TABLE_FULL: printf("Error: table full.\n"); break;
        }
    }
}
