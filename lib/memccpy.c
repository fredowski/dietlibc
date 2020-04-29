#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <sys/types.h>
#include <string.h>

void *memccpy(void *dst, const void *src, int c, size_t count)
{
  unsigned char *a = dst;
  const unsigned char *b = src;
  while (count--)
  {
    *a++ = *b;
    if (*b==(unsigned char)c)
    {
      return (void *)a;
    }
    b++;
  }
  return 0;
}
