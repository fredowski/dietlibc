#include <sys/tls.h>
#include <threads.h>

thrd_t thrd_current(void) {
  tcbhead_t* x=__get_cur_tcb();
//  if (x->multiple_threads==0) return _thrd_root;
  return (thrd_t)(x+1);
}
