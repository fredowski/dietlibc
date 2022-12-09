#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define _GNU_SOURCE
#include <utmpx.h>

void updwtmpx(const char *wtmpx_file, const struct utmpx *ut) {
  int fd = open(wtmpx_file, O_WRONLY|O_APPEND|O_CLOEXEC);
  if (fd<0) return;
  write(fd, ut, sizeof(struct utmpx));
  close(fd);
}
