#ifndef _MACHINE_ATOMIC_H_
#define _MACHINE_ATOMIC_H_

#define atomic_cas_uint(p, o, n) __sync_val_compare_and_swap((p), (o), (n))
#define atomic_cas_ulong(p, o, n) __sync_val_compare_and_swap((p), (o), (n))
#define atomic_cas_ptr(p, o, n) __sync_val_compare_and_swap((p), (o), (n))

#define atomic_swap_uint(p, v) __sync_lock_test_and_set((p), (v))
#define atomic_swap_ulong(p, v) __sync_lock_test_and_set((p), (v))
#define atomic_swap_ptr(p, v) __sync_lock_test_and_set((p), (v))

#define atomic_add_int_nv(p, v) __sync_add_and_fetch((p), (v))
#define atomic_add_long_nv(p, v) __sync_add_and_fetch((p), (v))

#define atomic_sub_int_nv(p, v) __sync_sub_and_fetch((p), (v))
#define atomic_sub_long_nv(p, v) __sync_sub_and_fetch((p), (v))

#define membar_sync() __sync_synchronize()
#define membar_enter() __sync_synchronize()
#define membar_exit() __sync_synchronize()
#define membar_producer() __sync_synchronize()
#define membar_consumer() __sync_synchronize()

#endif /* _MACHINE_ATOMIC_H_ */
