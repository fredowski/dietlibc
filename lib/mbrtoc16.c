#include <uchar.h>
#include <errno.h>
#include "dietmb.h"

/* Read one UTF-8 character from s, but consume at most n bytes.
 * If that is only a partial UTF-8 sequence, return -2.
 * If it is an invalid UTF-8 sequence, set errno to EILSEQ and return -1.
 * If it is 0, then return 0.
 * Convert that character to UTF-16 (possibly a surrogate pair)
 * and write one UTF-16 character to pwc, return number of bytes
 * consumed from s.
 * If we still had the second half of a surrogate pair to be written,
 * write that and return -3.
 */

size_t mbrtoc16(char16_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps) {
  /* if ps is NULL, use a static internal state */
  static mbstate_t internal;
  if (!ps) ps=&internal;

  /* first check if we still have the second surrogate pair to write */
  if (ps->count==10) {
    ps->count=0;
    if (pwc) *pwc=0xdc00 + ps->sofar;
    return -3;
  }
  /* read one character */
  uint32_t x;
  ssize_t r=__mbrtox(s,n,&x);
  if (r<0) {
    if (r==-1) errno=EILSEQ;
    return r;
  }
  if (x==0) return 0;
  /* the surrogate pair ranges are reserved in input */
  if (x>=0xd800 && x<=0xdfff) {
    errno=EILSEQ;
    return -1;
  }
  /* now see if we need a surrogate pair */
  if (x >= 0x10000) {
    if (x > 0x10ffff) {		// these are technically encodable in UTF-8 and our decoder can read them
      errno=EILSEQ;		// but they are not representable in UTF-16 and should not occur in the first place
      return -1;
    }
    x-=0x10000;
    ps->count=10;
    ps->sofar=x&0x3ff;
    if (pwc) *pwc=0xd800 + ((x >> 10) & 0x3ff);
    return r;
  }
  if (pwc) *pwc=x;
  return r;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

#undef UNITTEST
#include "lib/mbrtox.c"

int main() {
  char16_t x;
  mbstate_t s = { 0 };
  /* first some positive tests */
  assert(mbrtoc16(&x,"\x7a",1,&s)==1 && x==0x7a);
  assert(mbrtoc16(&x,"\xc3\x9f",2,&s)==2 && x==0xdf);
  assert(mbrtoc16(&x,"\xe6\xb0\xb4",3,&s)==3 && x==0x6c34);
  assert(mbrtoc16(&x,"\xf0\x9f\x8d\x8c",4,&s)==4 && x==0xd83c);
  assert(mbrtoc16(&x,"\xf0\x9f\x8d\x8c"+4,0,&s)==-3 && x==0xdf4c);

  /* now some negative tests */
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc16(&x,"\x8f",1,&s)==-1 && errno==EILSEQ);	// first byte continuation
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc16(&x,"\xc3",1,&s)==-2);		// incomplete sequence
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc16(&x,"\xe6\xb0",2,&s)==-2);		// incomplete sequence
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc16(&x,"\xe6 ",2,&s)==-1);		// invalid and incomplete, expect invalid
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc16(&x,"\xfe\xff",2,&s)==-1);		// BOM not valid in UTF-8
}
#endif
