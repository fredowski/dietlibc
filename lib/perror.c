#include "dietfeatures.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define _BSD_SOURCE
#include <errno.h>

void perror(const char *s) {
  register const char *message="[unknown error]";
  register int save=errno;
  write(2,s,strlen(s));
  write(2,": ",2);
  if (save>=0 && save<sys_nerr)
    message=sys_errlist[save];
  write(2,message,strlen(message));
  write(2,"\n",1);
}
