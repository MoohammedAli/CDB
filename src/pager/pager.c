#include "pager.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

Pager* pager_open (const char* filename) {
    int fd = open(filename, 
                  O_RDWR |     // Read / write mode
                     O_CREAT,  // Create file if it doesnt exist
                  S_IWUSR |    // User write permission
                     S_IRUSR); // User read permission

    if (fd == -1) {
        printf("Unable to open file.");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);
    Pager* pager = malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;
    for (uint32_t i=0; i<TABLE_MAX_PAGE; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

void pager_flush (Pager* pager, uint32_t page_num, uint32_t size) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
    if (bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}


void* get_page (Pager* pager, uint32_t page_num) {
    if (page_num >= TABLE_MAX_PAGE) {
        printf("Tried to fetch page number out of bound. %d > %d\n", page_num, TABLE_MAX_PAGE);
        exit(EXIT_FAILURE);
    }
    if (pager->pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;

        // We might save a partial page at the end of the file.
        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }
    return pager->pages[page_num];
}

void pager_close (Pager* pager) {
    int result = close(pager->file_descriptor);
    if (result == -1) { printf("Error closing db file. \n"); exit(EXIT_FAILURE); }
    for (uint32_t i = 0; i < TABLE_MAX_PAGE; i++) {
        if (pager->pages[i]) {
            free(pager->pages[i]);
            pager->pages[i] = NULL;
        }
    } 
    free(pager);
}
