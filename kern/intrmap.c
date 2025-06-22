#include <stdlib.h>
#include <sys/intrmap.h>

struct device;

/* Opaque forward declarations, you can include actual headers if available */
struct device;
struct cpu_info {
	unsigned int cpu_id; /* example field */
};

/*
 * Internal structure for intrmap.
 * We'll store:
 * - reference to device,
 * - count of CPUs,
 * - a simple array of cpu_info pointers
 * - flags (e.g. INTRMAP_POWEROF2)
 */
struct intrmap {
	const struct device *device;
	unsigned int flags;
	unsigned int cpu_count;
	struct cpu_info **cpus;
};

/*
 * Create a new intrmap instance.
 * Arguments:
 *  - device: the device pointer this map is for
 *  - flags: bit flags (e.g. INTRMAP_POWEROF2)
 *  - cpu_count: number of CPUs in the map
 *  - cpu_base_id: starting CPU id for this map (used to create dummy cpu_info here)
 */
struct intrmap *
intrmap_create(const struct device *device, unsigned int flags,
    unsigned int cpu_count, unsigned int cpu_base_id)
{
	struct intrmap *im;
	unsigned int i;

	im = malloc(sizeof(*im));
	if (im == NULL)
		return NULL;

	im->device = device;
	im->flags = flags;
	im->cpu_count = cpu_count;

	im->cpus = malloc(sizeof(struct cpu_info *) * cpu_count);
	if (im->cpus == NULL) {
		free(im);
		return NULL;
	}

	/* For demo purposes, allocate dummy cpu_info structs */
	for (i = 0; i < cpu_count; i++) {
		im->cpus[i] = malloc(sizeof(struct cpu_info));
		if (im->cpus[i] == NULL) {
			/* Cleanup on failure */
			while (i > 0) {
				free(im->cpus[--i]);
			}
			free(im->cpus);
			free(im);
			return NULL;
		}
		im->cpus[i]->cpu_id = cpu_base_id + i;
	}

	return im;
}

/*
 * Destroy an intrmap instance and free resources.
 */
void
intrmap_destroy(struct intrmap *im)
{
	unsigned int i;

	if (im == NULL)
		return;

	for (i = 0; i < im->cpu_count; i++) {
		free(im->cpus[i]);
	}
	free(im->cpus);
	free(im);
}

/*
 * Return the count of CPUs in this intrmap.
 */
unsigned int
intrmap_count(const struct intrmap *im)
{
	if (im == NULL)
		return 0;
	return im->cpu_count;
}

/*
 * Return the cpu_info pointer for the CPU at the given index.
 */
struct cpu_info *
intrmap_cpu(const struct intrmap *im, unsigned int idx)
{
	if (im == NULL || idx >= im->cpu_count)
		return NULL;
	return im->cpus[idx];
}
