#define _REENTRANT
#include <threads.h>
#include <sys/futex.h>
#include <sys/cdefs.h>

int mtx_unlock(mtx_t* mutex) {
  /* https://en.cppreference.com/w/c/thread/mtx_unlock
   * "The behavior is undefined if the mutex is not locked by the calling thread."
   * glibc doesn't care. FreeBSD returns thrd_error */
  if (__unlikely(mutex->owner != thrd_current()))
    return thrd_error;

  int cur,v;
  do {
    cur=mutex->lock;
    if (__unlikely(cur==0))	// wasn't locked
      return thrd_error;
    v = cur-2;
  } while (__unlikely(!__sync_bool_compare_and_swap(&mutex->lock,cur,v)));
  if (v>1)
    return thrd_success;	// recursive lock, others still holding
  mutex->owner=0;
  if (cur&1)	// non-empty futex queue
    if (futex(&mutex->lock,FUTEX_WAKE_PRIVATE,1,0,0,0)==0)
      __sync_bool_compare_and_swap(&mutex->lock,1,0);
  return thrd_success;
}
