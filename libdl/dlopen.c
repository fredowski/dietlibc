#include <fcntl.h>
#include <dlfcn.h>
#include <limits.h>

#include "_dl_int.h"

void *dlopen(const char *filename, int flags)
{
  if (filename)
    return _dl_open(filename,flags);
  /* dietld.so has allocated the top for the dynamic program.
   * (if there is a dietld.so :) not yet functional
   * (started the implementation) )*/
  return _dl_root_handle;
}
