#include <threads.h>

int cnd_init(cnd_t* cond) {
  cond->seq=0;
  cond->mtx=0;
  return thrd_success;
}
