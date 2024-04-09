#include "dietdirent.h"
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>

DIR*  opendir ( const char* name ) {
  int   fd = open (name, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
  DIR*  t  = NULL;

  if ( fd >= 0 ) {
    t = (DIR *) mmap (NULL, DIRSTREAMSIZE, PROT_READ | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (t == MAP_FAILED)
      close (fd);
    else
      t->fd = fd;
  }


  return t;
}
