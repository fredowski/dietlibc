#include <string.h>

void* explicit_memset(void* dest, int c, size_t len) {
  memset(dest,c,len);
  asm volatile("": : "r"(dest) : "memory");
  return dest;
}

void* memset_explicit(void* dest, int c, size_t n) __attribute__((alias("explicit_memset")));
