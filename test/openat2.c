#define _GNU_SOURCE
#include <fcntl.h>
#include <linux/openat2.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

int main() {
  struct open_how how = { .flags = O_RDWR, .resolve = RESOLVE_IN_ROOT };
  int fd=openat2(AT_FDCWD, "../", &how, sizeof(how));
}
