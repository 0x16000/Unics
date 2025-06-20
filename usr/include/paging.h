#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <pmm.h>

// Entries per page table (assuming 32-bit x86 with 4KB pages)
#define PAGE_ENTRIES 1024

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

// A page table (1024 entries)
typedef struct page_table {
    page_entry_t entries[PAGE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

// A page directory (1024 entries)
typedef struct page_directory {
    page_entry_t entries[PAGE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

// Current page directory pointer
extern page_directory_t *current_page_directory;

// Initialize paging subsystem
void paging_init(void);

// Map a virtual address to a physical address with given flags
bool paging_map_page(uintptr_t virt_addr, uintptr_t phys_addr, uint32_t flags);

// Unmap a virtual address
bool paging_unmap_page(uintptr_t virt_addr);

// Get the physical address mapped to a virtual address, or 0 if none
uintptr_t paging_get_physical(uintptr_t virt_addr);

// Enable paging by loading the page directory and setting CR0 (x86-specific)
void paging_enable(void);

// Invalidate a page in the TLB
void paging_invalidate(uintptr_t virt_addr);

#endif // PAGING_H
