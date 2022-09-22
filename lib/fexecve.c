#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

extern int __ltostr(char *s, unsigned int size, unsigned long i, unsigned int base, int UpCase);

int fexecve(int fd, char* const argv[], char* const envp[]) {
  int r=execveat(fd, "", (const char* const*)argv, (const char* const*)envp, AT_EMPTY_PATH);
  if (r == ENOSYS) {
    char buf[30];
    memcpy(buf,"/proc/self/fd/",14);
    buf[14+__ltostr(buf+14,30-14,fd,10,0)]=0;
    return execve(buf, argv, envp);
  }
  return r;
}
