#ifndef PMM_H
#define PMM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PMM_PAGE_SIZE 4096
#define PMM_BITMAP_BITS (sizeof(uint64_t) * 8)  // 64 bits per bitmap entry

// Memory zone definitions
typedef enum {
    PMM_ZONE_DMA = 0,      // 0-16MB for DMA-capable devices
    PMM_ZONE_NORMAL,       // 16MB-896MB normal memory
    PMM_ZONE_HIGH,         // >896MB high memory
    PMM_ZONE_COUNT
} pmm_zone_t;

// Memory statistics
typedef struct {
    size_t total_pages;
    size_t free_pages;
    size_t allocated_pages;
    size_t reserved_pages;
    size_t zones_pages[PMM_ZONE_COUNT];
    size_t zones_free[PMM_ZONE_COUNT];
} pmm_stats_t;

// Memory region descriptor
typedef struct pmm_region {
    uintptr_t base;
    size_t pages;
    uint64_t *bitmap;
    size_t bitmap_size;
    size_t last_alloc;  // Hint for next allocation
    pmm_zone_t zone;
    struct pmm_region *next;
} pmm_region_t;

// Initialization and management
int pmm_init(void);
int pmm_add_region(uintptr_t base, size_t size, pmm_zone_t zone);
int pmm_reserve_range(uintptr_t start, uintptr_t end);

// Page allocation/deallocation
void* pmm_alloc_page(void);
void* pmm_alloc_pages(size_t count);
void* pmm_alloc_page_zone(pmm_zone_t zone);
void* pmm_alloc_pages_zone(size_t count, pmm_zone_t zone);
int pmm_free_page(void* addr);
int pmm_free_pages(void* addr, size_t count);

// Utility functions
bool pmm_is_page_allocated(void* addr);
pmm_stats_t pmm_get_stats(void);
void pmm_dump_state(void);
void pmm_dump_stats(void);

// Address validation
static inline bool pmm_is_page_aligned(uintptr_t addr) {
    return (addr & (PMM_PAGE_SIZE - 1)) == 0;
}

static inline uintptr_t pmm_page_align_down(uintptr_t addr) {
    return addr & ~(PMM_PAGE_SIZE - 1);
}

static inline uintptr_t pmm_page_align_up(uintptr_t addr) {
    return (addr + PMM_PAGE_SIZE - 1) & ~(PMM_PAGE_SIZE - 1);
}

#endif // PMM_H
