#ifndef _WCHAR_H
#define _WCHAR_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

__BEGIN_DECLS

#if defined(__WINT_TYPE__)
typedef __WINT_TYPE__ wint_t;
#else
typedef unsigned int wint_t;
#endif
typedef int (*wctype_t)(wint_t);

#ifndef WCHAR_MIN
#define WCHAR_MIN (-2147483647 - 1)
#define WCHAR_MAX (2147483647)
#endif
#ifndef WEOF
#define WEOF 0xffffffffu
#endif

struct tm;

#ifndef __mbstate_t_declared
#define __mbstate_t_declared
typedef __CHAR32_TYPE__ char32_t;
typedef struct {
  int count;
  char32_t sofar;
} mbstate_t;
#endif

wint_t btowc(int);
wint_t fgetwc(FILE *);
wchar_t* fgetws(wchar_t *__restrict__, int, FILE *__restrict__);
wint_t fputwc(wchar_t, FILE *);
int fputws(const wchar_t *__restrict__, FILE *__restrict__);
int fwide(FILE *, int);
int fwprintf(FILE *__restrict__, const wchar_t *__restrict__, ...);
int fwscanf(FILE *__restrict__, const wchar_t *__restrict__, ...);
wint_t getwc(FILE *);
wint_t getwchar(void);

__readmemsz__(1,2)
size_t mbrlen(const char *__restrict__, size_t, mbstate_t *__restrict__);
__readmemsz__(2,3)
size_t mbrtowc(wchar_t *__restrict__, const char *__restrict__, size_t, mbstate_t *__restrict__);
int mbsinit(const mbstate_t *);
__writememsz__(1,3)
size_t mbsrtowcs(wchar_t *__restrict__, const char **__restrict__, size_t, mbstate_t *__restrict__);
wint_t putwc(wchar_t, FILE *);
wint_t putwchar(wchar_t);
__writememsz__(1,2)
int swprintf(wchar_t *__restrict__, size_t, const wchar_t *__restrict__, ...);
int swscanf(const wchar_t *__restrict__, const wchar_t *__restrict__, ...);

wint_t ungetwc(wint_t, FILE *);
int vfwprintf(FILE *__restrict__, const wchar_t *__restrict__, va_list);
int vfwscanf(FILE *__restrict__, const wchar_t *__restrict__, va_list);
int vwprintf(const wchar_t *__restrict__, va_list);
__writememsz__(1,2)
int vswprintf(wchar_t *__restrict__, size_t, const wchar_t *__restrict__, va_list);
int vswscanf(const wchar_t *__restrict__, const wchar_t *__restrict__, va_list);
int vwscanf(const wchar_t *__restrict__, va_list);
size_t wcrtomb(char *__restrict__, wchar_t, mbstate_t *__restrict__);
__writemem__(1) __readmem__(2)
wchar_t *wcscat(wchar_t *__restrict__, const wchar_t *__restrict__);
__readmem__(1)
wchar_t *wcschr(const wchar_t *, wchar_t);
__readmem__(1) __readmem__(2)
int wcscmp(const wchar_t *, const wchar_t *);
__readmem__(1) __readmem__(2)
int wcscoll(const wchar_t *, const wchar_t *);
__writemem__(1) __readmem__(2)
wchar_t* wcscpy(wchar_t *__restrict__, const wchar_t *__restrict__);
__readmem__(1) __readmem__(2)
size_t wcscspn(const wchar_t *, const wchar_t *);
__writememsz__(1,2)
size_t wcsftime(wchar_t *__restrict__, size_t, const wchar_t *__restrict__, const struct tm *__restrict__);
__readmem__(1)
size_t wcslen(const wchar_t *) __pure;
__writememsz__(1,3)
__readmem__(2)
wchar_t *wcsncat(wchar_t *__restrict__, const wchar_t *__restrict__, size_t);
__readmemsz__(1,3) __readmemsz__(2,3)
int wcsncmp(const wchar_t *, const wchar_t *, size_t);
__writememsz__(1,3) __readmemsz__(2,3)
wchar_t *wcsncpy(wchar_t *__restrict__, const wchar_t *__restrict__, size_t);
__readmem__(1) __readmem__(2)
wchar_t *wcspbrk(const wchar_t *, const wchar_t *);
__readmem__(1)
wchar_t *wcsrchr(const wchar_t *, wchar_t);
__writememsz__(1,3)
size_t wcsrtombs(char *__restrict__, const wchar_t **__restrict__, size_t, mbstate_t *__restrict__);
__readmem__(1) __readmem__(2)
size_t wcsspn(const wchar_t *, const wchar_t *);
__readmem__(1) __readmem__(2)
wchar_t *wcsstr(const wchar_t *__restrict__, const wchar_t *__restrict__);
__readmem__(1)
double wcstod(const wchar_t *__restrict__, wchar_t **__restrict__);
__readmem__(1)
float wcstof(const wchar_t *__restrict__, wchar_t **__restrict__);
__readwritemem__(1) __readmem__(2)
wchar_t *wcstok(wchar_t *__restrict__, const wchar_t *__restrict__, wchar_t **__restrict__);
__readmem__(1)
long wcstol(const wchar_t *__restrict__, wchar_t **__restrict__, int);
__readmem__(1)
long double wcstold(const wchar_t *__restrict__, wchar_t **__restrict__);
__readmem__(1)
long long wcstoll(const wchar_t *__restrict__, wchar_t **__restrict__, int);
__readmem__(1)
unsigned long wcstoul(const wchar_t *__restrict__, wchar_t **__restrict__, int);
__readmem__(1)
unsigned long long wcstoull(const wchar_t *__restrict__, wchar_t **__restrict__, int);

size_t wcsxfrm(wchar_t *__restrict__, const wchar_t *__restrict__, size_t);
int wctob(wint_t);

__readmemsz__(1,3)
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t);
__readmemsz__(1,3) __readmemsz__(2,3)
int wmemcmp(const wchar_t *, const wchar_t *, size_t);
__writememsz__(1,3) __readmemsz__(2,3)
wchar_t *wmemcpy(wchar_t *__restrict__, const wchar_t *__restrict__, size_t);
__writememsz__(1,3) __readmemsz__(2,3)
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t);
__writememsz__(1,3)
wchar_t *wmemset(wchar_t *, wchar_t, size_t);
int wprintf(const wchar_t *__restrict__, ...);
int wscanf(const wchar_t *__restrict__, ...);

#ifdef _XOPEN_SOURCE
int wcwidth(wchar_t c);
int wcswidth(const wchar_t *s, size_t n);
#endif

__END_DECLS

#endif
