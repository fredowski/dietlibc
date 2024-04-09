#include <sys/shm.h>
#include <dirent.h>
#include "syscalls.h"

union __dirent {
  struct dirent d;
#ifdef __NR_getdents64
  struct dirent64 d64;
#endif
};

#if PAGE_SIZE == 4096
#define DIRSTREAMSIZE PAGE_SIZE*2
#else
#define DIRSTREAMSIZE PAGE_SIZE
#endif

struct __dirstream {
  union __dirent d;
  char buf[PAGE_SIZE-(sizeof (int)*3)-sizeof(union __dirent)];
  int fd;
  unsigned int num;
  unsigned int cur;
};				/* stream data from opendir() */
