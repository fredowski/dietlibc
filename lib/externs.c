#include "dietfeatures.h"
#include <stddef.h>
#include <sys/tls.h>

#ifdef WANT_GNU_STARTUP_BLOAT
char* program_invocation_name;
char* program_invocation_short_name;
#endif

void* __vdso;

#ifdef __i386__
int __modern_linux;
#endif

#ifdef WANT_TLS
/* __tdatasize is the size of the initialized thread local data section
 * __tmemsize is the size of the complete thread local data section
 *   (including uninitialized data)
 * __tdataptr is a pointer to the initialized thread local data section
 * __tmemsize is already rounded up to meet alignment
 * the final memory layout is [tdata] [tbss (zero)] [tcb] */
size_t __tdatasize, __tmemsize;
void* __tdataptr;
#endif

#if defined(WANT_SSP) || defined(WANT_TLS)
tcbhead_t* __tcb_mainthread;
#endif
