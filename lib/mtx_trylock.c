#define _REENTRANT
#define _DIETLIBC_SOURCE
#include <threads.h>
#include <sys/futex.h>
#include <sys/cdefs.h>

int __mtx_trylock(mtx_t* mutex,int* lockval) {
  int i;
  /* https://en.cppreference.com/w/c/thread/mtx_lock
   * "The behavior is undefined if the current thread has already locked
   * the mutex and the mutex is not recursive."
   * glibc and FreeBSD return thrd_busy, so do we. */

  /* attempt to lock the mutex with an atomic instruction */
  i=mutex->lock;
  if (i<2) {
    if (__likely((__sync_val_compare_and_swap(&mutex->lock,i,i+2))==i)) {
      mutex->owner=thrd_current();
      return thrd_success;
    }
  }
  if ((mutex->type&mtx_recursive)) {
    if (__unlikely(mutex->owner!=thrd_current()))
      return thrd_busy;
    if (__unlikely((__sync_add_and_fetch(&mutex->lock,2)>>1) > 1000)) {
      __sync_add_and_fetch(&mutex->lock,-2);
      return thrd_error;
    }
    return thrd_success;
  }
  if (lockval) *lockval=i;
  return thrd_busy;
}

int mtx_trylock(mtx_t* mutex) {
  return __mtx_trylock(mutex,NULL);
}
