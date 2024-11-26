/* this needs to be a .S to override syscalls.h/epoll_create.S :( */
.text
.global epoll_wait
.type epoll_wait, %function
epoll_wait:
  mov x4, 0
  b epoll_pwait
.size epoll_wait, .-epoll_wait
.section .note.GNU-stack,"",@progbits
