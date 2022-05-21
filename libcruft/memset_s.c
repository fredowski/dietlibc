#include <string.h>
#include <errno.h>

/* C11 Annex K is failed garbage that glibc actively refuses to implement.
 * I concur with their judgement. I provide a memset_s here because
 * explicit_bzero is non-standard and wanting a memset that won't get
 * optimized out is a legitimate request */
errno_t memset_s( void *dest, rsize_t destsz, int ch, rsize_t count ) {
  if (destsz < count || destsz > RSIZE_MAX || !dest)
    return errno=EINVAL;
  explicit_bzero(dest, count);
  return 0;
}
