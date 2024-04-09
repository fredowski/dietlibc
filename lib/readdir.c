#define _POSIX_SOURCE
#include "dietdirent.h"
#include <unistd.h>
#include <dirent.h>

struct dirent* readdir(DIR *d) {
  struct dirent* ld;
  if (readdir_r(d,&d->d.d,&ld)) return 0;
  return ld;
}
