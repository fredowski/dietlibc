#include "dietstdio.h"
#include <unistd.h>
#include <fcntl.h>
#include "binshstr.h"

extern char **environ;

FILE *popen(const char *command, const char *type) {
  int pfd[2];
  int fd0;
  FILE* f;
  pid_t pid;

  if (pipe(pfd)<0) return 0;
  fd0=(*type=='r');
  if ((!(f=fdopen(pfd[!fd0],type))) ||
      ((pid=fork())<0)) {
    close(pfd[0]);	/* malloc failed */
    close(pfd[1]);
    return 0;
  }
  if (!pid) {	/* child */
    const char *argv[]={__sh,"-c",0,0};
    if (close(pfd[!fd0])==-1 ||
	close(fd0)==-1 ||
	dup2(pfd[fd0],fd0)==-1 ||
	close(pfd[fd0])==-1)
      _exit(127);
    argv[2]=command;
    execve(__binsh,(char*const*)argv,environ);
    _exit(127);
  }
  close(pfd[fd0]);
  fcntl (pfd[!fd0], F_SETFD, FD_CLOEXEC);
  f->popen_kludge=pid;
  return f;
}
