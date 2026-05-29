#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE       = size_of_attribute(ROW, id);
const uint32_t USERNAME_SIZE = size_of_attribute(ROW, username);
const uint32_t EMAIL_SIZE    = size_of_attribute(ROW, email);
const uint32_t ID_OFFSET     = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET    = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE        = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t ROWS_PER_PAGE   = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS  = ROWS_PER_PAGE * TABLE_MAX_PAGE;

void print_row(ROW* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void serialize_row(ROW* source, void* destination) {
    memcpy(destination + ID_OFFSET,       &(source->id),       ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET,    &(source->email),    EMAIL_SIZE);
}

void deserialize_row(void* source, ROW* destination) {
    memcpy(&(destination->id),       source + ID_OFFSET,       ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email),    source + EMAIL_OFFSET,    EMAIL_SIZE);
}

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = get_page(table->pager, page_num);
    uint32_t row_offset  = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

Table* db_open(const char* filename) {
    Pager* pager = pager_open(filename);
    uint32_t num_rows = pager->file_length / ROW_SIZE;
    Table* table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;
    return table;
}

void db_close(Table* table) {
    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) continue;
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    pager_close(pager);
    free(table);
}

Cursor* table_start(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);
    return cursor;
}

Cursor* table_end(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;
    return cursor;
}
