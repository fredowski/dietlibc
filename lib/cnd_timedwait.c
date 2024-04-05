#include <threads.h>
#include <sys/futex.h>
#include <errno.h>

int cnd_timedwait(cnd_t* cond, mtx_t* mutex, const struct timespec* time_point) {
  int r;
  do {
    r=futex(&cond->sem,FUTEX_WAIT_BITSET_PRIVATE|FUTEX_CLOCK_REALTIME,0,time_point,0,FUTEX_BITSET_MATCH_ANY);
    if (r==-1) {
      if (errno==EWOULDBLOCK) break;
      else if (errno==EINTR) continue;
    } else
      break;
  } while (r==0);
  cond->sem=0;
  return mtx_timedlock(mutex,time_point);
}
