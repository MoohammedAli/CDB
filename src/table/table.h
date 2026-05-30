#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "../pager/pager.h"

#define COLUMN_USERNAME_SIZE 128
#define COLUMN_EMAIL_SIZE 255

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} ROW;

typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;

typedef struct {
    Table* table;
    uint32_t row_num;
    bool end_of_table;
} Cursor;

// row layout constants
extern const uint32_t ROW_SIZE;
extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

Table* db_open(const char* filename);
void db_close(Table* table);
void* cursor_value(Cursor* cursor);
void cursor_advance(Cursor* cursor);
void serialize_row(ROW* source, void* destination);
void deserialize_row(void* source, ROW* destination);
Cursor* table_start(Table* table);
Cursor* table_end(Table* table);
void print_row(ROW* row);

#endif
