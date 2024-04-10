#define _REENTRANT
#include <threads.h>
#include <sys/futex.h>

int mtx_lock(mtx_t* mutex) {
  return mtx_timedlock(mutex,0);
}
