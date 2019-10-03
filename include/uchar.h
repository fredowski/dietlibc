#ifndef _UCHAR_H
#define _UCHAR_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdio.h>

__BEGIN_DECLS

#ifndef __STDC_UTF_16__
#define __STDC_UTF_16__
#endif
#ifndef __STDC_UTF_32__
#define __STDC_UTF_32__
#endif

typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;

#ifndef __mbstate_t_declared
#define __mbstate_t_declared
typedef struct {
  int count;
  char32_t sofar;
} mbstate_t;
#endif

size_t c16rtomb(char *restrict s, char16_t wc, mbstate_t *restrict ps) __THROW;
size_t c32rtomb(char *restrict s, char32_t wc, mbstate_t *restrict ps) __THROW;
size_t mbrtoc16(char16_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps) __THROW;
size_t mbrtoc32(char32_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps) __THROW;

__END_DECLS

#endif
