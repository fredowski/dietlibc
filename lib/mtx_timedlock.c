#define _REENTRANT
#define _DIETLIBC_SOURCE
#include <threads.h>
#include <sys/futex.h>
#include <errno.h>

int mtx_timedlock(mtx_t* mutex, const struct timespec* time_point) {
  int i,r;
  do {
    r=__mtx_trylock(mutex,&i);
    if (r!=thrd_busy) return r;
    for (;;) {
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
