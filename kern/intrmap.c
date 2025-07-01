#include <stdlib.h>
#include <sys/intrmap.h>

struct device;

struct cpu_info {
    unsigned int cpu_id;
};

struct intrmap {
    const struct device *device;
    unsigned int flags;
    unsigned int cpu_count;
    struct cpu_info **cpus;
};

struct intrmap *
intrmap_create(const struct device *device, unsigned int flags,
               unsigned int cpu_count, unsigned int cpu_base_id)
{
    struct intrmap *im = malloc(sizeof(*im));
    if (!im)
        return NULL;

    im->device = device;
    im->flags = flags;
    im->cpu_count = cpu_count;

    im->cpus = malloc(sizeof(*im->cpus) * cpu_count);
    if (!im->cpus) {
        free(im);
        return NULL;
    }

    for (unsigned int i = 0; i < cpu_count; i++) {
        im->cpus[i] = malloc(sizeof(struct cpu_info));
        if (!im->cpus[i]) {
            while (i--)
                free(im->cpus[i]);
            free(im->cpus);
            free(im);
            return NULL;
        }
        im->cpus[i]->cpu_id = cpu_base_id + i;
    }

    return im;
}

void
intrmap_destroy(struct intrmap *im)
{
    if (!im)
        return;

    for (unsigned int i = 0; i < im->cpu_count; i++)
        free(im->cpus[i]);

    free(im->cpus);
    free(im);
}

unsigned int
intrmap_count(const struct intrmap *im)
{
    return im ? im->cpu_count : 0;
}

struct cpu_info *
intrmap_cpu(const struct intrmap *im, unsigned int idx)
{
    if (!im || idx >= im->cpu_count)
        return NULL;
    return im->cpus[idx];
}
