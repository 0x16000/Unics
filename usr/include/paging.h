#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pmm.h>

#define PAGE_ENTRIES   1024
#define PAGE_SIZE      4096

// Page flags
#define PAGE_PRESENT    0x001
#define PAGE_WRITABLE   0x002
#define PAGE_USER       0x004
#define PAGE_WRITETHRU  0x008
#define PAGE_NOCACHE    0x010
#define PAGE_ACCESSED   0x020
#define PAGE_DIRTY      0x040
#define PAGE_PAT        0x080
#define PAGE_GLOBAL     0x100

typedef uint32_t page_entry_t;

typedef struct page_table {
    page_entry_t entries[PAGE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef struct page_directory {
    page_entry_t entries[PAGE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

extern page_directory_t *current_page_directory;

void paging_init(void);
bool paging_map_page(uintptr_t virt_addr, uintptr_t phys_addr, uint32_t flags);
bool paging_unmap_page(uintptr_t virt_addr);
uintptr_t paging_get_physical(uintptr_t virt_addr);
void paging_enable(void);
void paging_invalidate(uintptr_t virt_addr);

#endif // _PAGING_H_
