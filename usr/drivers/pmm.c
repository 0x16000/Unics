#include <pmm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Global state
static pmm_region_t *pmm_regions = NULL;
static bool pmm_initialized = false;

// Built-in CLZ for systems without hardware support
static inline int clz64(uint64_t x) {
    if (x == 0) return 64;
    int n = 0;
    if (x <= 0x00000000FFFFFFFFULL) { n += 32; x <<= 32; }
    if (x <= 0x0000FFFFFFFFFFFFULL) { n += 16; x <<= 16; }
    if (x <= 0x00FFFFFFFFFFFFFFULL) { n += 8;  x <<= 8;  }
    if (x <= 0x0FFFFFFFFFFFFFFFULL) { n += 4;  x <<= 4;  }
    if (x <= 0x3FFFFFFFFFFFFFFFULL) { n += 2;  x <<= 2;  }
    if (x <= 0x7FFFFFFFFFFFFFFFULL) { n += 1; }
    return n;
}

// Fast bit manipulation using 64-bit words
static inline void set_bit_64(uint64_t *bitmap, size_t bit) {
    bitmap[bit / 64] |= (1ULL << (bit % 64));
}

static inline void clear_bit_64(uint64_t *bitmap, size_t bit) {
    bitmap[bit / 64] &= ~(1ULL << (bit % 64));
}

static inline bool test_bit_64(const uint64_t *bitmap, size_t bit) {
    return (bitmap[bit / 64] >> (bit % 64)) & 1;
}

// Find first zero bit in a 64-bit word (returns 64 if all bits set)
static inline int find_first_zero_bit_64(uint64_t word) {
    return clz64(~word);
}

// Find region containing the given address
static pmm_region_t* find_region_for_addr(uintptr_t addr) {
    for (pmm_region_t *region = pmm_regions; region; region = region->next) {
        if (addr >= region->base && 
            addr < region->base + region->pages * PMM_PAGE_SIZE) {
            return region;
        }
    }
    return NULL;
}

// Initialize PMM subsystem
int pmm_init(void) {
    if (pmm_initialized) {
        return -1; // Already initialized
    }
    
    pmm_regions = NULL;
    pmm_initialized = true;
    
    printf("pmm: Physical Memory Manager initialized\n");
    return 0;
}

// Add a memory region to management
int pmm_add_region(uintptr_t base, size_t size, pmm_zone_t zone) {
    if (!pmm_initialized) {
        return -1;
    }
    
    // Align base and size to page boundaries
    uintptr_t aligned_base = pmm_page_align_up(base);
    size_t aligned_size = pmm_page_align_down(size - (aligned_base - base));
    size_t pages = aligned_size / PMM_PAGE_SIZE;
    
    if (pages == 0) {
        return -1; // Region too small
    }
    
    // Allocate region descriptor (you'll need to implement a simple allocator for this)
    pmm_region_t *region = malloc(sizeof(pmm_region_t));
    if (!region) return -1;
    
    // Calculate bitmap size (in 64-bit words)
    size_t bitmap_size = (pages + 63) / 64;
    region->bitmap = calloc(bitmap_size, sizeof(uint64_t));
    if (!region->bitmap) {
        free(region);
        return -1;
    }
    
    region->base = aligned_base;
    region->pages = pages;
    region->bitmap_size = bitmap_size;
    region->last_alloc = 0;
    region->zone = zone;
    region->next = pmm_regions;
    pmm_regions = region;
    
    printf("pmm: Added region 0x%08lx - 0x%08lx (%zu pages, zone %d)\n",
           aligned_base, aligned_base + pages * PMM_PAGE_SIZE, pages, zone);
    
    return 0;
}

// Reserve a range of memory (mark as allocated)
int pmm_reserve_range(uintptr_t start, uintptr_t end) {
    if (!pmm_initialized) {
        return -1;
    }
    
    uintptr_t aligned_start = pmm_page_align_down(start);
    uintptr_t aligned_end = pmm_page_align_up(end);
    
    for (uintptr_t addr = aligned_start; addr < aligned_end; addr += PMM_PAGE_SIZE) {
        pmm_region_t *region = find_region_for_addr(addr);
        if (region) {
            size_t page_idx = (addr - region->base) / PMM_PAGE_SIZE;
            if (page_idx < region->pages) {
                set_bit_64(region->bitmap, page_idx);
            }
        }
    }
    
    printf("pmm: Reserved range 0x%08lx - 0x%08lx\n", aligned_start, aligned_end);
    return 0;
}

// Fast allocation within a region using bitmap scanning
static void* alloc_page_from_region(pmm_region_t *region) {
    size_t start_word = region->last_alloc / 64;
    
    // Search from last allocation point
    for (size_t word_idx = start_word; word_idx < region->bitmap_size; word_idx++) {
        uint64_t word = region->bitmap[word_idx];
        if (word != ~0ULL) { // Not all bits set
            int bit_offset = find_first_zero_bit_64(word);
            if (bit_offset < 64) {
                size_t page_idx = word_idx * 64 + bit_offset;
                if (page_idx < region->pages) {
                    set_bit_64(region->bitmap, page_idx);
                    region->last_alloc = page_idx;
                    return (void*)(region->base + page_idx * PMM_PAGE_SIZE);
                }
            }
        }
    }
    
    // Wrap around to beginning
    for (size_t word_idx = 0; word_idx < start_word; word_idx++) {
        uint64_t word = region->bitmap[word_idx];
        if (word != ~0ULL) {
            int bit_offset = find_first_zero_bit_64(word);
            if (bit_offset < 64) {
                size_t page_idx = word_idx * 64 + bit_offset;
                if (page_idx < region->pages) {
                    set_bit_64(region->bitmap, page_idx);
                    region->last_alloc = page_idx;
                    return (void*)(region->base + page_idx * PMM_PAGE_SIZE);
                }
            }
        }
    }
    
    return NULL; // Region is full
}

// Allocate a single page from any zone
void* pmm_alloc_page(void) {
    return pmm_alloc_page_zone(PMM_ZONE_NORMAL);
}

// Allocate a single page from specific zone
void* pmm_alloc_page_zone(pmm_zone_t zone) {
    if (!pmm_initialized) {
        return NULL;
    }
    
    // Try preferred zone first
    for (pmm_region_t *region = pmm_regions; region; region = region->next) {
        if (region->zone == zone) {
            void *page = alloc_page_from_region(region);
            if (page) return page;
        }
    }
    
    // Fallback to any zone if preferred zone is full
    for (pmm_region_t *region = pmm_regions; region; region = region->next) {
        void *page = alloc_page_from_region(region);
        if (page) return page;
    }
    
    return NULL; // Out of memory
}

// Allocate multiple contiguous pages
void* pmm_alloc_pages(size_t count) {
    return pmm_alloc_pages_zone(count, PMM_ZONE_NORMAL);
}

void* pmm_alloc_pages_zone(size_t count, pmm_zone_t zone) {
    if (!pmm_initialized || count == 0) {
        return NULL;
    }
    
    for (pmm_region_t *region = pmm_regions; region; region = region->next) {
        if (region->zone != zone) continue;
        
        // Search for contiguous free pages
        size_t consecutive = 0;
        size_t start_page = 0;
        
        for (size_t i = 0; i < region->pages; i++) {
            if (!test_bit_64(region->bitmap, i)) {
                if (consecutive == 0) start_page = i;
                consecutive++;
                if (consecutive == count) {
                    // Found enough consecutive pages, allocate them
                    for (size_t j = start_page; j < start_page + count; j++) {
                        set_bit_64(region->bitmap, j);
                    }
                    return (void*)(region->base + start_page * PMM_PAGE_SIZE);
                }
            } else {
                consecutive = 0;
            }
        }
    }
    
    return NULL; // No contiguous block found
}

// Free a single page
int pmm_free_page(void* addr) {
    return pmm_free_pages(addr, 1);
}

// Free multiple pages
int pmm_free_pages(void* addr, size_t count) {
    if (!pmm_initialized || !addr || count == 0) {
        return -1;
    }
    
    uintptr_t target = (uintptr_t)addr;
    if (!pmm_is_page_aligned(target)) {
        printf("[pmm] ERROR: Address 0x%08lx is not page-aligned\n", target);
        return -1;
    }
    
    pmm_region_t *region = find_region_for_addr(target);
    if (!region) {
        printf("[pmm] ERROR: Address 0x%08lx not in managed memory\n", target);
        return -1;
    }
    
    size_t start_page = (target - region->base) / PMM_PAGE_SIZE;
    if (start_page + count > region->pages) {
        printf("[pmm] ERROR: Free range extends beyond region boundary\n");
        return -1;
    }
    
    // Check for double-free
    for (size_t i = 0; i < count; i++) {
        if (!test_bit_64(region->bitmap, start_page + i)) {
            printf("[pmm] WARNING: Double-free detected at page %zu\n", start_page + i);
            return -1;
        }
    }
    
    // Free the pages
    for (size_t i = 0; i < count; i++) {
        clear_bit_64(region->bitmap, start_page + i);
    }
    
    // Update allocation hint
    if (start_page < region->last_alloc) {
        region->last_alloc = start_page;
    }
    
    return 0;
}

// Check if a page is allocated
bool pmm_is_page_allocated(void* addr) {
    if (!pmm_initialized || !addr) {
        return false;
    }
    
    uintptr_t target = (uintptr_t)addr;
    pmm_region_t *region = find_region_for_addr(target);
    if (!region) {
        return false;
    }
    
    size_t page_idx = (target - region->base) / PMM_PAGE_SIZE;
    return test_bit_64(region->bitmap, page_idx);
}

// Get memory statistics
pmm_stats_t pmm_get_stats(void) {
    pmm_stats_t stats = {0};
    
    if (!pmm_initialized) {
        return stats;
    }
    
    for (pmm_region_t *region = pmm_regions; region; region = region->next) {
        stats.total_pages += region->pages;
        stats.zones_pages[region->zone] += region->pages;
        
        // Count allocated pages
        size_t allocated = 0;
        for (size_t i = 0; i < region->pages; i++) {
            if (test_bit_64(region->bitmap, i)) {
                allocated++;
            }
        }
        
        stats.allocated_pages += allocated;
        size_t free = region->pages - allocated;
        stats.free_pages += free;
        stats.zones_free[region->zone] += free;
    }
    
    return stats;
}

// Dump detailed state information
void pmm_dump_state(void) {
    if (!pmm_initialized) {
        printf("[pmm] Not initialized\n");
        return;
    }
    
    printf("[pmm] Memory regions:\n");
    for (pmm_region_t *region = pmm_regions; region; region = region->next) {
        printf("  Region: 0x%08lx - 0x%08lx (%zu pages, zone %d)\n",
               region->base, region->base + region->pages * PMM_PAGE_SIZE,
               region->pages, region->zone);
        
        size_t allocated = 0;
        for (size_t i = 0; i < region->pages; i++) {
            if (test_bit_64(region->bitmap, i)) {
                allocated++;
            }
        }
        printf("    Allocated: %zu/%zu pages (%.1f%%)\n",
               allocated, region->pages, 
               (float)allocated / region->pages * 100.0f);
    }
}

// Dump memory statistics
void pmm_dump_stats(void) {
    pmm_stats_t stats = pmm_get_stats();
    
    printf("[pmm] Memory Statistics:\n");
    printf("  Total pages: %zu (%zu MB)\n", 
           stats.total_pages, stats.total_pages * PMM_PAGE_SIZE / (1024 * 1024));
    printf("  Free pages:  %zu (%zu MB)\n", 
           stats.free_pages, stats.free_pages * PMM_PAGE_SIZE / (1024 * 1024));
    printf("  Allocated:   %zu (%zu MB)\n", 
           stats.allocated_pages, stats.allocated_pages * PMM_PAGE_SIZE / (1024 * 1024));
    
    const char *zone_names[] = {"DMA", "NORMAL", "HIGH"};
    for (int i = 0; i < PMM_ZONE_COUNT; i++) {
        if (stats.zones_pages[i] > 0) {
            printf("  Zone %s: %zu total, %zu free (%.1f%% free)\n",
                   zone_names[i], stats.zones_pages[i], stats.zones_free[i],
                   (float)stats.zones_free[i] / stats.zones_pages[i] * 100.0f);
        }
    }
}
