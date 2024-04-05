#ifndef _SYS_FUTEX_H
#define _SYS_FUTEX_H

#include <sys/types.h>
#include <sys/time.h>

__BEGIN_DECLS

enum {
  FUTEX_WAIT=0,
  FUTEX_WAKE=1,
  FUTEX_FD=2,
  FUTEX_REQUEUE=3,
  FUTEX_CMP_REQUEUE=4,
  FUTEX_WAKE_OP=5,
  FUTEX_LOCK_PI=6,
  FUTEX_UNLOCK_PI=7,
  FUTEX_TRYLOCK_PI=8,
  FUTEX_WAIT_BITSET=9,
  FUTEX_WAKE_BITSET=10,
  FUTEX_WAIT_REQUEUE_PI=11,
  FUTEX_CMP_REQUEUE_PI=12,
  FUTEX_LOCK_PI2=13,
};

long futex(int* uaddr,int op,int val,const struct timespec* timeout,int* uaddr2,int val3);

__END_DECLS

#endif
