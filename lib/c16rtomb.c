#include <uchar.h>
#include <errno.h>

/* convert one utf-16 char to utf-8 */
/* if it is a surrogate pair, we just eat the char, update our state and
 * don't output anything */

/* state (in ps) can be NULL, in which case we use a static var */

/* s can be NULL, in which case we don't write anything, but pretend to
 * write 0 to an imaginary buffer, we reset the state, and return how
 * many bytes we would have written */

/* we ignore the locale and always do UTF-8 */

size_t c16rtomb(char *restrict s, char16_t wc, mbstate_t *restrict ps) {
  /* handle ps==NULL by switching to an internal state */
  static mbstate_t internal;
  char32_t w;
  if (!ps) ps=&internal;

  /* utf-16 uses "surrogate pairs" to encode sequences that do not fit
   * into 16 bits. You start by substracting 0x10000, leaving 20 bits.
   * Then you write two pairs:
   *   (0xd800 + the first 10 bits, 0xdc00 + the second 10 bits)
   */

  /* is it the first half of a surrogate pair? */
  if (wc >= 0xd800 && wc <= 0xdbff) {
    /* if we already have a surrogate pair in our state, this is
     * invalid. */
    if (ps->count) {
      errno=EILSEQ;
      return -1;
    }
    ps->count = 10;
    ps->sofar = (wc & 0x3ff);	// extract lower 10 bits
    return 0;
  }

  /* is it the second half of a surrogate pair? */
  if (wc >= 0xdc00 && wc <= 0xdfff) {
    /* if we did not already have a surrogate pair in our state, this is
     * invalid. */
    if (ps->count != 10) {
      errno=EILSEQ;
      return -1;
    }
    /* take the lower 10 bits from this surrogate pair, and the upper 10
     * bits from the previous surrogate pair in the state, then add the
     * implicit 0x10000 lower bounds for surrogate-pair encoded values */
    w = (wc & 0x3ff) + (ps->sofar << 10) + 0x10000;
    ps->count = 0;

    /* the UTF-8 encoding for this range is fixed: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (s) {
      s[0]=0xf0 | ((w >> 18) & 7);	// three bits
      s[1]=0x80 | ((w >> 12) & 0x3f);	// six bits
      s[2]=0x80 | ((w >> 6) & 0x3f);	// next six bits
      s[3]=0x80 | (w & 0x3f);		// last six bits
    }
    return 4;
  }

  /* if we were waiting for the second half of a surrogate pair, this
   * is invalid. */
  if (ps->count) {
    errno=EILSEQ;
    return -1;
  }
  w = wc;	/* just take wc as a code point */

  if (w >= 0x800) {
    /* case 3: 0x800 - 0xffff: 1110xxxx 10xxxxxx 10xxxxxx */
    if (s) {
      s[0]=0xe0 | ((w >> 12) & 0xf);	// four bits
      s[1]=0x80 | ((w >> 6) & 0x3f);	// six bits
      s[2]=0x80 | (w & 0x3f);		// last six bits
    }
    return 3;
  }
  if (w >= 0x80) {
    /* case 2: 0x80 - 0x7ff: 110xxxxx 10xxxxxx */
    if (s) {
      s[0]=0xc0 | ((w >> 6) & 0x1f);	// five bits
      s[1]=0x80 | (w & 0x3f);		// last six bits
    }
    return 2;
  }
  /* case 1: 0-0x7f: 0xxxxxxx */
  if (s)
    s[0]=w;
  return 1;
}

#ifdef UNITTEST
#include <limits.h>
#include <assert.h>
#include <string.h>

int main() {
  char buf[MB_LEN_MAX];
  mbstate_t st = { 0 };
  /* first some positive tests */
  assert(c16rtomb(buf, 0x7a, &st) == 1 && !memcmp(buf,"\x7a",1));
  assert(c16rtomb(buf, 0xdf, &st) == 2 && !memcmp(buf,"\xc3\x9f",2));
  assert(c16rtomb(buf, 0x6c34, &st) == 3 && !memcmp(buf,"\xe6\xb0\xb4",3));
  assert(c16rtomb(buf, 0xd83c, &st) == 0);
  assert(c16rtomb(buf, 0xdf4c, &st) == 4 && !memcmp(buf,"\xf0\x9f\x8d\x8c",4));

  /* now check error cases */
  assert(c16rtomb(buf, 0xdf4c, &st) == -1 && errno==EILSEQ);	// second half of pair without first half seen

  memset(&st,0,sizeof(st));
  assert(c16rtomb(buf, 0xd83c, &st) == 0);
  assert(c16rtomb(buf, 0xd83c, &st) == -1 && errno==EILSEQ);	// first half twice

  memset(&st,0,sizeof(st));
  assert(c16rtomb(buf, 0xd83c, &st) == 0);
  assert(c16rtomb(buf, 'x', &st) == -1 && errno==EILSEQ);	// second half of pair missing
}
#endif
