#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>

void __restore_rt(void);
asm(".text\n" ".align 16\n"
    "__restore_rt:"
    "mov x8,#139\n" "svc 0\n");	// copied from musl. I have no idea what this means.

int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  struct sigaction *newact = (struct sigaction *)act;
  if (act) {
	newact = alloca(sizeof(*newact));
	newact->sa_handler = act->sa_handler;
	newact->sa_flags = act->sa_flags | SA_RESTORER;
	newact->sa_restorer = &__restore_rt;
	newact->sa_mask = act->sa_mask;
  }
  return __rt_sigaction(signum, newact, oldact, _NSIG/8);
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
__attribute__((weak,alias("__libc_sigaction")));
