#include <string.h>

/* return 1 if the regions match */
/* always compare all bytes to avoid timing side channels */
int consttime_memequal(void* a,void* b,size_t n) {
  size_t i;
  const unsigned char* x = a;
  const unsigned char* y = b;
  int r;
  for (i=r=0; i<n; ++i)
    r |= x[i] ^ y[i];	/* x[i]^y[i] is 0 iff x[i]==y[i] */
  return r==0;
}

#ifdef UNITTEST
#include <assert.h>
int main() {
  assert(consttime_memequal("fnord","fnord",5)==1);
  assert(consttime_memequal("fnord","fnorD",5)==0);
}
#endif
