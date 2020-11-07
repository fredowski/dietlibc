#include <uchar.h>
#include <errno.h>

/* convert one utf-32 char to utf-8 */

/* we don't need and don't use the state */

/* s can be NULL, in which case we return 0 */

/* we ignore the locale and always do UTF-8 */

size_t c32rtomb(char *restrict s, char32_t w, mbstate_t *restrict ps) {
  (void)ps;	// not needed for char32_t

  /* the UTF-16 surrogate pair range is reserved */
  if (w >= 0xd800 && w <= 0xdfff) {
    errno=EILSEQ;
    return -1;
  }

  if (w >= 0x10000) {
    /* case 4: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (s) {
      s[0]=0xf0 | ((w >> 18) & 7);	// three bits
      s[1]=0x80 | ((w >> 12) & 0x3f);	// six bits
      s[2]=0x80 | ((w >> 6) & 0x3f);	// next six bits
      s[3]=0x80 | (w & 0x3f);		// last six bits
    }
    return 4;
  }
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
  assert(c32rtomb(buf, 0x7a, &st) == 1 && !memcmp(buf,"\x7a",1));
  assert(c32rtomb(buf, 0xdf, &st) == 2 && !memcmp(buf,"\xc3\x9f",2));
  assert(c32rtomb(buf, 0x6c34, &st) == 3 && !memcmp(buf,"\xe6\xb0\xb4",3));
  assert(c32rtomb(buf, 0x1F34C, &st) == 4 && !memcmp(buf,"\xf0\x9f\x8d\x8c",4));

  /* now check error cases */
  assert(c32rtomb(buf, 0xd83c, &st) == -1);	// surrogate pair range is invalid
}
#endif
