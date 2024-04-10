#define _REENTRANT
#define _DIETLIBC_SOURCE
#include <threads.h>
#include <sys/futex.h>
#include <errno.h>

int mtx_timedlock(mtx_t* mutex, const struct timespec* time_point) {
  int i,r,n=0;
  do {
    r=__mtx_trylock(mutex,&i);
    if (r!=thrd_busy)
      return r;
    if (++n < 100) {
#if defined(__x86__) || defined(__x86_64__)
      __asm__ __volatile__("pause" ::: "memory");
#endif
      __sync_synchronize();
      r=0;
      continue;
    }
    for (;;) {
      // i is the value in mutex->lock when __mtx_trylock gave up
      // set lowest bit in mutex->lock to signal mtx_unlock needs to do FUTEX_WAKE
      if ((i&1)==0)
	if (__unlikely(!__sync_bool_compare_and_swap(&mutex->lock,i,i+1)))
	  break;	// someone else already did it.
			// break out of for loop, continue do-while loop
      i+=1;
      if (time_point)
	r=futex(&mutex->lock,FUTEX_WAIT_BITSET_PRIVATE|FUTEX_CLOCK_REALTIME,i,time_point,0,FUTEX_BITSET_MATCH_ANY);
      else	// strace cosmetics
	r=futex(&mutex->lock,FUTEX_WAIT_PRIVATE,i,0,0,0);
      if (r==-1) {
	if (errno==EWOULDBLOCK) { r=0; break; } else
	if (errno==ETIMEDOUT) return thrd_timedout; else
	if (errno==EINTR) continue;
      } else
	break;
    }
  } while (r==0);
  return thrd_error;
}
