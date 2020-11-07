#include <uchar.h>
#include <errno.h>
#include "dietmb.h"

/* Read one UTF-8 character from s, but consume at most n bytes.
 * If that is only a partial UTF-8 sequence, return -2.
 * If it is an invalid UTF-8 sequence, set errno to EILSEQ and return -1.
 * If it is 0, then return 0.
 * Write that character to pwd if it is not NULL, return number of bytes
 * consumed from s.
 * We do not need (or use) the state.
 */

size_t mbrtoc32(char32_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps) {
  /* read one character */
  uint32_t x;
  ssize_t r=__mbrtox(s,n,&x);

  (void)ps;	// not needed for char32_t

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
  if (pwc) *pwc=x;
  return r;
}

#ifdef UNITTEST
#include <assert.h>
#include <string.h>

#undef UNITTEST
#include "lib/mbrtox.c"

int main() {
  char32_t x;
  mbstate_t s = { 0 };
  /* first some positive tests */
  assert(mbrtoc32(&x,"\x7a",1,&s)==1 && x==0x7a);
  assert(mbrtoc32(&x,"\xc3\x9f",2,&s)==2 && x==0xdf);
  assert(mbrtoc32(&x,"\xe6\xb0\xb4",3,&s)==3 && x==0x6c34);
  assert(mbrtoc32(&x,"\xf0\x9f\x8d\x8c",4,&s)==4 && x==0x1F34C);

  /* now some negative tests */
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc32(&x,"\x8f",1,&s)==-1 && errno==EILSEQ);	// first byte continuation
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc32(&x,"\xc3",1,&s)==-2);		// incomplete sequence
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc32(&x,"\xe6\xb0",2,&s)==-2);		// incomplete sequence
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc32(&x,"\xe6 ",2,&s)==-1);		// invalid and incomplete, expect invalid
  errno=0; memset(&s,0,sizeof(s)); assert(mbrtoc32(&x,"\xfe\xff",2,&s)==-1);		// BOM not valid in UTF-8
}
#endif
