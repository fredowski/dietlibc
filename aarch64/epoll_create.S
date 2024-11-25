/* this needs to be a .S to override syscalls.h/epoll_create.S :( */
.text
.global epoll_create
.type epoll_create, %function
epoll_create:
  mov w0, 0
  b epoll_create1
.size epoll_create, .-epoll_create
.section .note.GNU-stack,"",@progbits
