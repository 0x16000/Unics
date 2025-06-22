#include <sys/srp.h>
#include <sys/refcnt.h>

/*
 * Initialize srp subsystem if needed.
 * Currently a no-op.
 */
void
srp_startup(void)
{
	/* no global state to initialize */
}

/*
 * Initialize the srp_gc structure.
 */
void
srp_gc_init(struct srp_gc *gc, void (*dtor)(void *, void *), void *cookie)
{
	gc->srp_gc_dtor = dtor;
	gc->srp_gc_cookie = cookie;
	refcnt_init(&gc->srp_gc_refcnt);
}

/*
 * Initialize an srp reference.
 */
void
srp_init(struct srp *srp)
{
	srp->ref = NULL;
}

/*
 * Atomically swap the pointer stored in srp.
 * Returns the old pointer.
 */
void *
srp_swap_locked(struct srp *srp, void *ptr)
{
	void *old = srp->ref;
	srp->ref = ptr;
	return old;
}

/*
 * Update the srp pointer under GC control.
 * Manages reference counts accordingly.
 */
void
srp_update_locked(struct srp_gc *gc, struct srp *srp, void *ptr)
{
	void *old;

	old = srp_swap_locked(srp, ptr);

	if (ptr != NULL)
		refcnt_take(&gc->srp_gc_refcnt);

	if (old != NULL) {
		if (refcnt_rele(&gc->srp_gc_refcnt)) {
			gc->srp_gc_dtor(gc->srp_gc_cookie, old);
			refcnt_finalize(&gc->srp_gc_refcnt, "srp_gc_finalize");
		}
	}
}

/*
 * Return the pointer stored in srp.
 */
void *
srp_get_locked(struct srp *srp)
{
	return srp->ref;
}

/*
 * Finalize the srp_gc object, release one reference and
 * call destructor if refcount hits zero.
 */
void
srp_gc_finalize(struct srp_gc *gc)
{
	if (refcnt_rele(&gc->srp_gc_refcnt)) {
		gc->srp_gc_dtor(gc->srp_gc_cookie, NULL);
		refcnt_finalize(&gc->srp_gc_refcnt, "srp_gc_finalize");
	}
}

#ifdef MULTIPROCESSOR
/*
 * Multiprocessor stubs calling locked versions.
 * Hazard pointers not implemented here.
 */

void *
srp_swap(struct srp *srp, void *ptr)
{
	return srp_swap_locked(srp, ptr);
}

void
srp_update(struct srp_gc *gc, struct srp *srp, void *ptr)
{
	srp_update_locked(gc, srp, ptr);
}

void
srp_finalize(void *ptr, const char *wchan)
{
	(void)ptr;
	(void)wchan;
}

void *
srp_enter(struct srp_ref *sr, struct srp *srp)
{
	/* Not implemented */
	return NULL;
}

void *
srp_follow(struct srp_ref *sr, struct srp *srp)
{
	/* Not implemented */
	return NULL;
}

void
srp_leave(struct srp_ref *sr)
{
	/* Not implemented */
}

#endif /* MULTIPROCESSOR */
