#include <sys/types.h>
#include <sys/refcnt.h>
#include <stdio.h>

/* Function prototypes */
unsigned int refcnt_read(struct refcnt *r);

void
refcnt_init(struct refcnt *r)
{
    r->r_refs = 1;
    r->r_traceidx = 0;
}

void
refcnt_init_trace(struct refcnt *r, int id)
{
    r->r_refs = 1;
    r->r_traceidx = id;
}

void
refcnt_take(struct refcnt *r)
{
    __sync_fetch_and_add(&r->r_refs, 1);
}

int
refcnt_rele(struct refcnt *r)
{
    if (__sync_sub_and_fetch(&r->r_refs, 1) == 0)
        return 1;
    return 0;
}

void
refcnt_rele_wake(struct refcnt *r)
{
    if (refcnt_rele(r)) {
        /* Add wakeup / cleanup logic here */
    }
}

void
refcnt_finalize(struct refcnt *r, const char *msg)
{
    if (refcnt_read(r) != 0) {
        printf("refcnt_finalize: %s: refcnt %u still held\n", msg, refcnt_read(r));
    }
}

int
refcnt_shared(struct refcnt *r)
{
    return (refcnt_read(r) > 1);
}

unsigned int
refcnt_read(struct refcnt *r)
{
    return __sync_add_and_fetch(&r->r_refs, 0);
}
