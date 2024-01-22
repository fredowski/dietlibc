#include <assert.h>
#include <stdckdint.h>

int main() {
  int i;
  long l;
  assert(!ckd_add(&i,1,3));
  assert(ckd_add(&i,0x7fffffff,3));
  assert(!ckd_add(&l,0x7fffffff,3));
  return 0;
}
