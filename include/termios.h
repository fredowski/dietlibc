#ifndef	_TERMIOS_H
#define	_TERMIOS_H

#include <sys/cdefs.h>
#include <sys/types.h>

#include <linux/termios.h>

pid_t tcgetpgrp(int fd) __THROW;
int tcsetpgrp(int fd, pid_t pgrpid) __THROW;
int tcgetattr(int fd, struct termios *termios_p) __THROW;
int tcsetattr(int fd, int optional_actions, struct termios *termios_p) __THROW;
speed_t cfgetospeed(struct termios *termios_p) __THROW;
int cfsetospeed(struct termios *termios_p, speed_t speed) __THROW;
speed_t cfgetispeed(struct termios *termios_p) __THROW;
int cfsetispeed(struct termios *termios_p, speed_t speed) __THROW;

#endif
