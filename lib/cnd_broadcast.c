#include <threads.h>
#include <sys/futex.h>

int cnd_broadcast(cnd_t* cond) {
  mtx_t* m=cond->mtx;
  if (!m) return thrd_success;	// no mutex means nobody queued up
  __sync_add_and_fetch(&cond->seq,1);
  // try to set the contention flag in the mutex, so mutex_unlock will FUTEX_WAKE
  int old=__sync_fetch_and_or(&m->lock,1);
  if (futex(&cond->seq,FUTEX_REQUEUE_PRIVATE,1,(void*)0x7fffffff,(void*)m,0)==0)
    // if we didn't requeue any threads, undo
    __sync_bool_compare_and_swap(&m->lock,1,old);
  return thrd_success;
}
