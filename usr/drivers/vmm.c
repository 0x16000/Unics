#include <vmm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static vmm_page_t page_table[VMM_MAX_PAGES];
static uintptr_t vmm_base_address = 0x100000; // Start after kernel

void vmm_init(void) {
    memset(page_table, 0, sizeof(page_table));
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        page_table[i].base_addr = vmm_base_address + i * VMM_PAGE_SIZE;
        page_table[i].size = VMM_PAGE_SIZE;
        page_table[i].state = VMM_PAGE_FREE;
        page_table[i].alloc_pages = 0;
        page_table[i].tag = 0;
    }
    printf("vmm0: Initialized %d pages starting at 0x%08x\n", VMM_MAX_PAGES, (unsigned)vmm_base_address);
}

void* vmm_alloc(size_t size) {
    size_t pages_needed = (size + VMM_PAGE_SIZE - 1) / VMM_PAGE_SIZE;

    for (size_t i = 0; i <= VMM_MAX_PAGES - pages_needed; ++i) {
        bool block_free = true;
        for (size_t j = 0; j < pages_needed; ++j) {
            if (page_table[i + j].state != VMM_PAGE_FREE) {
                block_free = false;
                break;
            }
        }
        if (block_free) {
            for (size_t j = 0; j < pages_needed; ++j) {
                page_table[i + j].state = VMM_PAGE_USED;
                page_table[i + j].alloc_pages = 0;
            }
            page_table[i].alloc_pages = pages_needed;
            return (void*)page_table[i].base_addr;
        }
    }

    return NULL; // No memory block large enough
}

void* vmm_alloc_aligned(size_t size, size_t align) {
    if (align < VMM_PAGE_SIZE) align = VMM_PAGE_SIZE;
    size_t pages_needed = (size + VMM_PAGE_SIZE - 1) / VMM_PAGE_SIZE;

    for (size_t i = 0; i <= VMM_MAX_PAGES - pages_needed; ++i) {
        if ((page_table[i].base_addr % align) != 0) continue;

        bool block_free = true;
        for (size_t j = 0; j < pages_needed; ++j) {
            if (page_table[i + j].state != VMM_PAGE_FREE) {
                block_free = false;
                break;
            }
        }

        if (block_free) {
            for (size_t j = 0; j < pages_needed; ++j) {
                page_table[i + j].state = VMM_PAGE_USED;
                page_table[i + j].alloc_pages = 0;
            }
            page_table[i].alloc_pages = pages_needed;
            return (void*)page_table[i].base_addr;
        }
    }

    return NULL;
}

void vmm_free(void* addr) {
    uintptr_t target = (uintptr_t)addr;

    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].base_addr == target && page_table[i].state == VMM_PAGE_USED) {
            size_t pages = page_table[i].alloc_pages;
            if (pages == 0) {
                printf("vmm0: Warning: Block at 0x%08x has no allocation size metadata!\n", (unsigned)target);
                return;
            }
            for (size_t j = 0; j < pages; ++j) {
                page_table[i + j].state = VMM_PAGE_FREE;
                page_table[i + j].alloc_pages = 0;
                page_table[i + j].tag = 0;
            }
            return;
        }
    }

    printf("vmm0: Warning: Tried to free unallocated memory at 0x%08x\n", (unsigned)target);
}

void vmm_dump_state(void) {
    printf("[vmm] Page table dump:\n");
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        printf("  Page %3zu: 0x%08x - %s", i, (unsigned)page_table[i].base_addr,
               page_table[i].state == VMM_PAGE_USED ? "USED" : "FREE");

        if (page_table[i].state == VMM_PAGE_USED && page_table[i].alloc_pages > 0)
            printf(" (%zu pages)", page_table[i].alloc_pages);

        printf("\n");
    }
}

void vmm_stats(size_t* used_kb, size_t* free_kb) {
    size_t used = 0, free = 0;
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].state == VMM_PAGE_USED) used++;
        else free++;
    }
    if (used_kb) *used_kb = used * (VMM_PAGE_SIZE / 1024);
    if (free_kb) *free_kb = free * (VMM_PAGE_SIZE / 1024);
}
