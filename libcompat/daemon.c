#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <paths.h>

#include "daemon.h"

int daemon (int nochdir,int noclose)
{
  int fd;
  int r=0;
  switch (fork()) {
  case -1: return (-1);
  case  0: break;
  default: _exit (0);
  }
  if (setsid () == -1) return (-1);
  if (!nochdir) chdir ("/");
  if (!noclose) {
    fd = open(_PATH_DEVNULL,O_RDWR,0);
    if (fd == -1) return (-1);
    if (dup2 (fd,STDIN_FILENO) == -1 ||
	dup2 (fd,STDOUT_FILENO) == -1 ||
	dup2 (fd,STDERR_FILENO) == -1) {
      r = -1;
    }
      
    if (fd>2 && close (fd) == -1) r = -1;
  }
  return r;
}

