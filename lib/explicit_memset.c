#include <string.h>

void* explicit_memset(void* dest, int c, size_t len) {
  memset(dest,c,len);
  asm volatile("": : "r"(dest) : "memory");
  return dest;
}
