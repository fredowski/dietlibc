#include <threads.h>
#include <sys/futex.h>
#include <errno.h>

int cnd_timedwait(cnd_t* cond, mtx_t* mutex, const struct timespec* time_point) {
  int r;
  if (cond->mtx != mutex) {
    if (__sync_val_compare_and_swap(&cond->mtx,0,mutex) != 0)
      return thrd_error;	// other mutex already set
  }
  if ((r=mtx_unlock(mutex)) != thrd_success) return r;
  do {
    int cur=cond->seq;
    if (time_point)
      r=futex(&cond->seq,FUTEX_WAIT_BITSET_PRIVATE|FUTEX_CLOCK_REALTIME,cur,time_point,0,FUTEX_BITSET_MATCH_ANY);
    else	// strace cosmetics
      r=futex(&cond->seq,FUTEX_WAIT_PRIVATE,cur,0,0,0);
    if (r==-1) {
      if (errno==EWOULDBLOCK) break;	// we said wait for value 0 but value was already 0
      else if (errno==EINTR) continue;
    } else
      break;
  } while (r==0);
  return mtx_timedlock(mutex,time_point);
}
