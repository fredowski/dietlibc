#include <signal.h>
#include <asm/errno.h>
#include <errno.h>

#define SHELL_PATH      "/bin/sh"       /* Path of the shell.  */
#define SHELL_NAME      "sh"            /* Name to give it.  */

extern char **environ;

int fork();
int waitpid(int pid, int *status, int options);
int execve(const char*filename, char *const argv[], char *const envp[]);
void __set_errno(int errno);
int sigaction(int signum,  const  struct  sigaction  *act, struct sigaction *oldact);

int system (const char *line)
{
  struct sigaction sa, intr, quit;
  int save,pid,ret=-1;

  if (line == 0) return system("exit 0") == 0;

  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigemptyset (&sa.sa_mask);

  if (sigaction(SIGINT,  &sa, &intr)<0) return -1;
  if (sigaction(SIGQUIT, &sa, &quit)<0) {
    save = errno;
    sigaction (SIGINT, &intr, (struct sigaction*)0);
    __set_errno (save);
    return -1;
  }

  pid=fork();
  if (pid>0)
  { /* parent */
    int n;
    do
      n=waitpid(pid, &ret, 0);
    while ((n==-1) && (errno==EINTR));
    if (n!=pid) ret=-1;
  }
  else if (!pid)
  { /* child */
    const char *nargs[4];
    nargs[0] = SHELL_NAME;
    nargs[1] = "-c";
    nargs[2] = line;
    nargs[3] = 0;

    sigaction(SIGINT,  &intr, (struct sigaction*)0);
    sigaction(SIGQUIT, &quit, (struct sigaction*)0);

    execve(SHELL_PATH,(char *const *)nargs, environ);
    _exit(127);
  }
  save = errno;
  sigaction (SIGINT,  &intr, (struct sigaction *)0);
  sigaction (SIGQUIT, &quit, (struct sigaction *)0);
  __set_errno(save);
  return ret;
}
