#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

int futimens(int fd, const struct timespec* times) {
  return utimensat(fd,NULL,times,0);
}

