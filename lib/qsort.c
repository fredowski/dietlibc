#define _GNU_SOURCE
#include <sys/cdefs.h>
#include <stdlib.h>
#include "rand_i.h"

static void exch(char* base,size_t size,size_t a,size_t b) {
  char* x=base+a*size;
  char* y=base+b*size;
  while (size) {
    char z=*x;
    *x=*y;
    *y=z;
    --size; ++x; ++y;
  }
}

#define RAND

/* Quicksort with 3-way partitioning, ala Sedgewick */
/* Blame him for the scary variable names */
/* http://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf */
static void quicksort(char* base,size_t size,ssize_t l,ssize_t r,
		      int (*compar)(const void*,const void*,void*),
		      void* cookie) {
  ssize_t i=l-1, j=r, p=l-1, q=r, k;
  char* v=base+r*size;
  if (r<=l) return;

#ifdef RAND
  /*
     We chose the rightmost element in the array to be sorted as pivot,
     which is OK if the data is random, but which is horrible if the
     data is already sorted.  Try to improve by exchanging it with a
     random other pivot.
   */
  exch(base,size,l+(rand_i()%(r-l)),r);
#elif defined MID
  /*
     We chose the rightmost element in the array to be sorted as pivot,
     which is OK if the data is random, but which is horrible if the
     data is already sorted.  Try to improve by chosing the middle
     element instead.
   */
  exch(base,size,l+(r-l)/2,r);
#endif

  for (;;) {
    while (++i != r && compar(base+i*size,v,cookie)<0) ;
    while (compar(v,base+(--j)*size,cookie)<0) if (j == l) break;
    if (i >= j) break;
    exch(base,size,i,j);
    if (compar(base+i*size,v,cookie)==0) exch(base,size,++p,i);
    if (compar(v,base+j*size,cookie)==0) exch(base,size,j,--q);
  }
  exch(base,size,i,r); j = i-1; ++i;
  for (k=l; k<p; k++, j--) exch(base,size,k,j);
  for (k=r-1; k>q; k--, i++) exch(base,size,i,k);
  quicksort(base,size,l,j,compar,cookie);
  quicksort(base,size,i,r,compar,cookie);
}

void qsort_r(void* base,size_t nmemb,size_t size,int (*compar)(const void*,const void*,void*),void* cookie) {
  /* check for integer overflows */
  size_t prod;
  if (nmemb >= (((size_t)-1)>>1) ||
      size >= (((size_t)-1)>>1) ||
      __builtin_mul_overflow(nmemb,size,&prod)) return;
  if (nmemb>1)
    quicksort(base,size,0,nmemb-1,compar,cookie);
}

void qsort(void* base,size_t nmemb,size_t size,int (*compar)(const void*,const void*)) {
  qsort_r(base,nmemb,size,(void*)compar,0);
}

