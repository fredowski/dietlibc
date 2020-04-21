#include <assert.h>
#include <stdlib.h>

int main() {
  int* x;
  assert(x=reallocarray(0, 10, 10));
  assert(!(x=reallocarray(x, (((size_t)-1)/4)+1, 4)));
}
