#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>

#define TABLE_MAX_PAGE 100
#define PAGE_SIZE 4096

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGE];
} Pager;

Pager* pager_open (const char* file_name);
void pager_flush (Pager* pager, uint32_t page_num, uint32_t size);
void* get_page (Pager* pager, uint32_t page_num);
void pager_close (Pager* pager);

#endif
