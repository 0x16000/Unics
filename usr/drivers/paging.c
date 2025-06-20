#include <paging.h>
#include <pmm.h>
#include <string.h>
#include <machine/atomic.h>

static page_directory_t *kernel_page_directory = NULL;
page_directory_t *current_page_directory = NULL;

// Helper to get page directory and table indices from virtual address
static inline size_t pd_index(uintptr_t addr) {
    return (addr >> 22) & 0x3FF;
}
static inline size_t pt_index(uintptr_t addr) {
    return (addr >> 12) & 0x3FF;
}

// Get pointer to page table for virtual address, create if create_if_missing is true
static page_table_t *paging_get_table(page_directory_t *pd, uintptr_t virt_addr, bool create_if_missing) {
    size_t pdi = pd_index(virt_addr);
    page_entry_t pde = pd->entries[pdi];

    if (pde & PAGE_PRESENT) {
        // Table is present: get its address (clear lower flags)
        return (page_table_t *)(pde & ~0xFFF);
    }

    if (!create_if_missing)
        return NULL;

    // Allocate a new page table
    void *new_table_phys = pmm_alloc_page();
    if (!new_table_phys)
        return NULL;

    memset(new_table_phys, 0, PAGE_SIZE);

    // Map new table into directory with present + writable flags
    uintptr_t new_table_phys_addr = (uintptr_t)new_table_phys;
    pd->entries[pdi] = new_table_phys_addr | PAGE_PRESENT | PAGE_WRITABLE;

    return (page_table_t *)new_table_phys_addr;
}

void paging_init(void) {
    // Allocate page directory
    kernel_page_directory = (page_directory_t *)pmm_alloc_page();
    if (!kernel_page_directory) {
        // Handle allocation failure (panic or similar)
        // For now, just return
        return;
    }
    memset(kernel_page_directory, 0, PAGE_SIZE);

    current_page_directory = kernel_page_directory;

    // You may want to identity map some kernel pages here, or map kernel higher half
    // but that depends on your OS design

    // Finally enable paging
    paging_enable();
}

bool paging_map_page(uintptr_t virt_addr, uintptr_t phys_addr, uint32_t flags) {
    if (!pmm_is_page_aligned(virt_addr) || !pmm_is_page_aligned(phys_addr))
        return false;

    page_table_t *pt = paging_get_table(current_page_directory, virt_addr, true);
    if (!pt)
        return false;

    size_t pti = pt_index(virt_addr);
    pt->entries[pti] = phys_addr | (flags & 0xFFF) | PAGE_PRESENT;

    // Invalidate TLB for this page
    paging_invalidate(virt_addr);

    return true;
}

bool paging_unmap_page(uintptr_t virt_addr) {
    if (!pmm_is_page_aligned(virt_addr))
        return false;

    page_table_t *pt = paging_get_table(current_page_directory, virt_addr, false);
    if (!pt)
        return false;

    size_t pti = pt_index(virt_addr);

    if (!(pt->entries[pti] & PAGE_PRESENT))
        return false;

    pt->entries[pti] = 0;

    paging_invalidate(virt_addr);

    return true;
}

uintptr_t paging_get_physical(uintptr_t virt_addr) {
    page_table_t *pt = paging_get_table(current_page_directory, virt_addr, false);
    if (!pt)
        return 0;

    size_t pti = pt_index(virt_addr);
    page_entry_t entry = pt->entries[pti];
    if (!(entry & PAGE_PRESENT))
        return 0;

    return entry & ~0xFFF;
}

void paging_invalidate(uintptr_t virt_addr) {
    // x86: Invalidate page in TLB
    asm volatile("invlpg (%0)" ::"r"(virt_addr) : "memory");
}

void paging_enable(void) {
    uintptr_t pd_phys = (uintptr_t)current_page_directory;

    // Load page directory address into CR3
    asm volatile("mov %0, %%cr3" :: "r"(pd_phys));

    // Enable paging bit in CR0
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
