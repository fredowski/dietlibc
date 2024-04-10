#define _REENTRANT
#include <threads.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#ifdef __dietlibc__
#include <write12.h>
#else
#include <string.h>
static void __write1(const char* s) {
  write(1,s,strlen(s));
}
#endif
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <inttypes.h>

mtx_t m;

long l;

int thread(void* arg) {
  size_t i;
  (void)arg;
  for (i=0; i<100; ++i) {
    mtx_lock(&m);
    ++l;
    mtx_unlock(&m);
  }
  return 23;
}

int thread2(void* arg) {
  volatile int* x=arg;
  mtx_lock(&m);
  *x=1;
  sleep(1);
  mtx_unlock(&m);
  return 23;
}

int recursive_lock(int a) {
  mtx_lock(&m);
  if (a>0)
    return recursive_lock(a-1);
  else
    sleep(1);
  mtx_unlock(&m);
  return 0;
}

int thread3(void* arg) {
  (void)arg;
  recursive_lock(5);
  return 0;
}

once_flag f=ONCE_FLAG_INIT;

void callme(void) {
  __write1("callme got called\n");
}

int thread4(void* arg) {
  (void)arg;
  call_once(&f,callme);
  return 0;
}

cnd_t cond;
int done;

int thread5(void* arg) {
  int me = (int)(uintptr_t)arg;
  (void)arg;
  mtx_lock(&m);
  cnd_wait(&cond,&m);
  ++done;
  mtx_unlock(&m);
  return 0;
}

int thread6(void* arg) {
  (void)arg;
  errno=0;
  assert(write(-1,"foo",3)==-1);
  assert(errno==EBADF);
  return 0;
}

int main() {
  thrd_t threads[16];
  size_t i;
  mtx_init(&m,mtx_plain);

  __write1("plain mutex test\n");
  for (i=0; i<sizeof(threads)/sizeof(threads[0]); ++i) {
    assert(thrd_create(threads+i,thread,NULL)==thrd_success);
  }
  for (i=0; i<sizeof(threads)/sizeof(threads[0]); ++i) {
    int r;
    assert(thrd_join(threads[i],&r)==thrd_success && r==23);
  }
  assert(l==100*sizeof(threads)/sizeof(threads[0]));

  __write1("mtx_timedlock test\n");
  volatile int n=0;
  assert(thrd_create(threads,thread2,(void*)&n)==thrd_success);
  while (n!=1) __sync_synchronize();
  {
    struct timespec ts;
    struct timeval tv;
    ts.tv_sec=0;
    ts.tv_nsec=100000;
    gettimeofday(&tv,0);
    ts.tv_sec=tv.tv_sec;
    ts.tv_nsec=tv.tv_usec*1000+100000;
    if (ts.tv_nsec>=1000000000) {
      ts.tv_nsec -= 1000000000;
      ts.tv_sec += 1;
    }
    assert(mtx_timedlock(&m,&ts)==thrd_timedout);
  }
  {
    int r;
    assert(thrd_join(threads[0],&r)==thrd_success);
  }
  mtx_destroy(&m);

  __write1("mtx_recursive test\n");
  mtx_init(&m,mtx_recursive);
  assert(thrd_create(threads,thread3,NULL)==thrd_success);
  {
    struct timespec ts;
    ts.tv_sec=0;
    ts.tv_nsec=10000;
    nanosleep(&ts,0);
    // at this point thread3 should be running, and it should have
    // locked the recursive lock, and be inside the sleep(1);
    assert(mtx_trylock(&m)==thrd_busy);
  }
  {
    int r;
    assert(thrd_join(threads[0],&r)==thrd_success);
  }

  __write1("call_once test\n");
  for (i=0; i<sizeof(threads)/sizeof(threads[0]); ++i) {
    assert(thrd_create(threads+i,thread4,NULL)==thrd_success);
  }
  for (i=0; i<sizeof(threads)/sizeof(threads[0]); ++i) {
    int r;
    assert(thrd_join(threads[i],&r)==thrd_success && r==0);
  }

  __write1("cnd_t test\n");
  cnd_init(&cond);
  mtx_init(&m,mtx_plain);
  done=0;
  for (i=0; i<sizeof(threads)/sizeof(threads[0]); ++i) {
    assert(thrd_create(threads+i,thread5,(void*)(size_t)i)==thrd_success);
    assert(thrd_detach(threads[i])==thrd_success);
  }
  {
    struct timespec ts;
    ts.tv_sec=0;
    ts.tv_nsec=100000000;
    nanosleep(&ts,0);
    cnd_signal(&cond);
    nanosleep(&ts,0);
    mtx_lock(&m);
    assert(done==1);
    mtx_unlock(&m);
    cnd_broadcast(&cond);
    nanosleep(&ts,0);
    mtx_lock(&m);
//    fprintf(stderr,"done=%d, expected %zu\n",done,sizeof(threads)/sizeof(threads[0]));
    assert(done==sizeof(threads)/sizeof(threads[0]));
    mtx_unlock(&m);
  }

  errno=0;
  assert(write(-1,"foo",3)==-1);
  assert(errno==EBADF);

  assert(thrd_create(threads,thread6,NULL)==thrd_success);
  assert(thrd_join(threads[0],NULL)==thrd_success);

  return 0;
}
