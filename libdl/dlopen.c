#include <fcntl.h>
#include <dlfcn.h>
#include <limits.h>

#include "_dl_int.h"

#ifdef __DIET_LD_SO__
static
#endif
void*_dlopen(const char *filename, int flags) {
  struct _dl_handle* ret;
  if (filename) {
    if ((ret=_dl_find_lib(filename))) {
      ++(ret->lnk_count);	/* add a reference */
      return ret;
    }
    return _dl_open(filename,flags);
  }
  /* dietld.so has allocated the top for the dynamic program. */
  return _dl_root_handle;
}


void*dlopen(const char *filename, int flags) {
  _dl_error_location="dlopen";
  return _dlopen(filename,flags|RTLD_USER|RTLD_NOSONAME);
}
//__attribute__((alias("_dlopen")));
