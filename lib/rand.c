#include <stdlib.h>

static unsigned int seed=1;

int rand(void) {
  return rand_r(&seed);
}

void srand(unsigned int i) { seed=i?i:23; }

int random(void) __attribute__((leaf, nothrow, alias("rand")));
void srandom(unsigned int i) __attribute__((leaf, nothrow, alias("srand")));
