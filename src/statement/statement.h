#ifndef STATEMENT_H
#define STATEMENT_H

#include "../table/table.h"
#include "../repl/repl.h"

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;
typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG
} PrepareResult;

typedef struct {
    StatementType type;
    ROW row_to_insert;
} Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
ExecuteResult execute_statement(Statement* statement, Table* table);

#endif
