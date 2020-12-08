#define _GNU_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int pthread_self() {
  return gettid();
}

