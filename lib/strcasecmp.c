#include <strings.h>

int  strcasecmp ( const char* s1, const char* s2 )
{
  size_t i;
  for (i=0; ; ++i) {
    register unsigned int x1=(unsigned char)s1[i];
    register unsigned int x2=(unsigned char)s2[i];
    if (__unlikely(x1 - 'A' < 26u)) x1 += 32;	// tolower
    if (__unlikely(x2 - 'A' < 26u)) x2 += 32;	// tolower
    register int r = x1 - x2;
    if (__unlikely(r)) return r;
    if (__unlikely(!x1)) return 0;
  }
}
