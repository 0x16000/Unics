#include <pmm.h>
#include <string.h>
#include <stdio.h>

static uint8_t pmm_bitmap[PMM_MAX_PAGES / 8];
static uintptr_t pmm_base_address = 0x100000; // Start at 1MB

// Internal helpers
static inline void set_bit(size_t index) {
    pmm_bitmap[index / 8] |= (1 << (index % 8));
}

static inline void clear_bit(size_t index) {
    pmm_bitmap[index / 8] &= ~(1 << (index % 8));
}

static inline bool test_bit(size_t index) {
    return (pmm_bitmap[index / 8] >> (index % 8)) & 1;
}

void pmm_init(void) {
    memset(pmm_bitmap, 0, sizeof(pmm_bitmap));
    printf("pmm0: Initialized physical memory manager for %d pages (%u KB)\n", 
           PMM_MAX_PAGES, (PMM_MAX_PAGES * PMM_PAGE_SIZE) / 1024);
}

void* pmm_alloc_page(void) {
    for (size_t i = 0; i < PMM_MAX_PAGES; ++i) {
        if (!test_bit(i)) {
            set_bit(i);
            return (void*)(pmm_base_address + i * PMM_PAGE_SIZE);
        }
    }
    return NULL; // Out of physical memory
}

void pmm_free_page(void* addr) {
    uintptr_t target = (uintptr_t)addr;

    if (target < pmm_base_address) {
        printf("[pmm] Warning: Attempt to free below base address: 0x%08x\n", (unsigned)target);
        return;
    }

    size_t index = (target - pmm_base_address) / PMM_PAGE_SIZE;
    if (index >= PMM_MAX_PAGES || !test_bit(index)) {
        printf("[pmm] Warning: Invalid or double free at 0x%08x\n", (unsigned)target);
        return;
    }

    clear_bit(index);
}

void pmm_dump_state(void) {
    printf("[pmm] Allocated pages:\n");
    for (size_t i = 0; i < PMM_MAX_PAGES; ++i) {
        if (test_bit(i)) {
            printf("  Page %5zu: 0x%08x\n", i, (unsigned)(pmm_base_address + i * PMM_PAGE_SIZE));
        }
    }
}
