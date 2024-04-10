#include <threads.h>
#include <sys/futex.h>

int cnd_signal(cnd_t* cond) {
  __sync_add_and_fetch(&cond->seq,1);
  futex(&cond->seq,FUTEX_WAKE_PRIVATE,1,0,0,0);
  return thrd_success;
}
