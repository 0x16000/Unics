#ifndef VMM_H
#define VMM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define VMM_PAGE_SIZE 4096
#define VMM_MAX_PAGES 1024

typedef enum {
    VMM_PAGE_FREE = 0,
    VMM_PAGE_USED = 1
} vmm_page_state_t;

typedef struct {
    uintptr_t base_addr;
    size_t size;
    vmm_page_state_t state;
    size_t alloc_pages;   // number of pages used in this block (set only on first page)
    uint32_t tag;         // optional tag for ownership/debug
} vmm_page_t;

void vmm_init(void);
void* vmm_alloc(size_t size);
void* vmm_alloc_aligned(size_t size, size_t align);
void vmm_free(void* addr);
void vmm_dump_state(void);
void vmm_stats(size_t* used_kb, size_t* free_kb);

#endif // VMM_H
