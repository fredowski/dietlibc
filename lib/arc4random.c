#define _LINUX_SOURCE
#include <unistd.h>
#include <sys/random.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

static uint32_t buf[256];
static unsigned int n;

/* These come from OpenBSD: */
uint32_t arc4random(void) {
  if (n==0) arc4random_buf(buf, sizeof buf);
  uint32_t r=buf[n];
  if (++n > sizeof(buf)/sizeof(buf[0])) n=0;
  return r;
}

void arc4random_buf(void* Buf, size_t N) {
  static int enosys=0;
  int old_errno=errno;
  int r;
  int fd=-1;
  while (N>0) {
    if (!enosys) 
      r=getrandom(Buf,N,0);
    else {
      // fallback code in case kernel doesn't have getrandom
      if (fd==-1) fd=open("/dev/urandom",O_RDONLY);
      if (fd==-1) abort();
      r=read(fd,Buf,N);
    }
    if (r<=0) {
      switch(errno) {
      case ENOSYS:
	enosys=1;	// kernel doesn't have getrandom (pre 3.17)
	/* fall through */
      case EINTR:
	continue;
      }
      abort();
    }
    Buf += r;	// getrandom has a limit of 32MiB, so loop if necessary
    N -= r;
  }
  if (fd!=-1) close(fd);
  errno=old_errno;	// man page doesn't mention changes to errno
}

uint32_t arc4random_uniform(uint32_t upper_bound) {
  uint32_t r, min;

  if (upper_bound < 2)
    return 0;

  /* 2**32 % x == (2**32 - x) % x */
  min = -upper_bound % upper_bound;

  /*
    * This could theoretically loop forever but each retry has
    * p > 0.5 (worst case, usually far better) of selecting a
    * number inside the range we need, so it should rarely need
    * to re-roll.
    */
  for (;;) {
    r = arc4random();
    if (r >= min)
      break;
  }

  return r % upper_bound;
}

#ifdef UNITTEST
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main() {
  const size_t n = 64*1024*1024;
  unsigned char* buf=malloc(n);	// 64 MB
  memset(buf,0,n);
  arc4random_buf(buf,n);
  size_t i;
  size_t buckets[256];
  for (i=0; i<256; ++i) buckets[i]=0;
  for (i=0; i<n; ++i)
    buckets[buf[i]]++;
  size_t l,h;
  l=h=buckets[0];
  // count how often each byte happened
  for (i=1; i<256; ++i) {
    if (buckets[i]<l) l=buckets[i];
    if (buckets[i]>h) h=buckets[i];
  }
  // difference between highest and lowest count should be < 10%
  assert(l*1000/h > 90);
}
#endif
