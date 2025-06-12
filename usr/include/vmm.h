#ifndef VMM_H
#define VMM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define VMM_PAGE_SIZE        4096
#define VMM_MAX_PAGES        1024
#define VMM_BASE_ADDRESS     0x100000  // Start after kernel
#define VMM_TAG_KERNEL       0xDEAD0000
#define VMM_TAG_USER         0xBEEF0000

typedef enum {
    VMM_PAGE_FREE = 0,
    VMM_PAGE_USED = 1,
    VMM_PAGE_RESERVED = 2
} vmm_page_state_t;

typedef struct {
    uintptr_t base_addr;
    size_t size;
    vmm_page_state_t state;
    size_t alloc_pages;   // Number of pages in this allocation (set only on first page)
    uint32_t tag;         // Tag for ownership/debug
    bool is_kernel;       // Kernel vs user allocation
} vmm_page_t;

// Initialization
void vmm_init(void);
void vmm_reserve_range(uintptr_t start, uintptr_t end);

// Allocation
void* vmm_alloc(size_t size);
void* vmm_alloc_aligned(size_t size, size_t align);
void* vmm_alloc_tagged(size_t size, uint32_t tag, bool is_kernel);

// Deallocation
void vmm_free(void* addr);

// Information
void vmm_dump_state(void);
void vmm_stats(size_t* used_kb, size_t* free_kb, size_t* reserved_kb);
size_t vmm_get_total_memory(void);
size_t vmm_get_largest_free_block(void);

// Debugging
void vmm_set_tag(void* addr, uint32_t tag);
uint32_t vmm_get_tag(void* addr);

#endif // VMM_H
