#include "syscalls.h"

#ifdef __NR_epoll_pwait
syscall(epoll_pwait,epoll_pwait)
#endif
