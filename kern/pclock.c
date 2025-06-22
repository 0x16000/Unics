#include <stdint.h>

#include <sys/pclock.h>
#include <sys/types.h>
#include <machine/atomic.h>

void
pc_lock_init(struct pc_lock *pcl)
{
    pcl->pcl_gen = 0;
}

unsigned int
pc_sprod_enter(struct pc_lock *pcl)
{
    return pcl->pcl_gen;
}

void
pc_sprod_leave(struct pc_lock *pcl, unsigned int gen)
{
    pcl->pcl_gen = gen + 1;
}

unsigned int
pc_mprod_enter(struct pc_lock *pcl)
{
    unsigned int gen;

    for (;;) {
        gen = pcl->pcl_gen;

        if ((gen & 1) == 0) {
            if (atomic_cas_uint(&pcl->pcl_gen, gen, gen + 1))
                return gen + 1;
        }
        /* Optionally yield CPU or pause */
    }
}

void
pc_mprod_leave(struct pc_lock *pcl, unsigned int gen)
{
    pcl->pcl_gen = gen + 1;
}

void
pc_cons_enter(struct pc_lock *pcl, unsigned int *pgen)
{
    unsigned int gen1, gen2;

    for (;;) {
        gen1 = pcl->pcl_gen;
        if ((gen1 & 1) != 0)
            continue;

        gen2 = pcl->pcl_gen;
        if (gen1 == gen2 && (gen2 & 1) == 0) {
            *pgen = gen2;
            return;
        }
    }
}

int
pc_cons_leave(struct pc_lock *pcl, unsigned int *pgen)
{
    unsigned int gen = pcl->pcl_gen;

    if (gen != *pgen) {
        *pgen = gen;
        return 1;
    }
    return 0;
}
