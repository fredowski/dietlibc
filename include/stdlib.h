#ifndef _STDLIB_H
#define _STDLIB_H

#ifndef __ASSEMBLER__

#include <sys/cdefs.h>
#include <sys/types.h>
#include <alloca.h>

__BEGIN_DECLS

__attribute__((__warn_unused_result__))
__attribute_alloc2__(1,2)
void *calloc(size_t nmemb, size_t size) __THROW __attribute_malloc__;

__attribute__((__warn_unused_result__))
__attribute_alloc__(1)
void *malloc(size_t size)  __THROW __attribute_malloc__;
void free(void *ptr) __THROW;

__attribute__((__warn_unused_result__))
__attribute_alloc__(2)
void *realloc(void *ptr, size_t size) __THROW __attribute_malloc__;

/* useful OpenBSD extension: */
__attribute__((__warn_unused_result__))
__attribute_alloc2__(2,3)
void* reallocarray(void* ptr, size_t nmemb, size_t size) __THROW __attribute_malloc__ __attribute_alloc2__(2,3);

__attribute__((__warn_unused_result__))
char *getenv(const char *name) __THROW __pure;

__attribute__((__warn_unused_result__))
int putenv(const char *string) __THROW;

__attribute__((__warn_unused_result__))
int setenv(const char *name, const char *value, int overwrite) __THROW;

__attribute__((__warn_unused_result__))
int unsetenv(const char *name) __THROW;

__attribute__((__warn_unused_result__))
int system (const char * string) __THROW;

__attribute__((__warn_unused_result__))
int atexit(void (*function)(void)) __THROW;

__attribute__((__warn_unused_result__))
float strtof(const char *nptr, char **endptr) __THROW;

double strtod(const char *nptr, char **endptr) __THROW;
long double strtold(const char *nptr, char **endptr) __THROW;
long int strtol(const char *nptr, char **endptr, int base) __THROW;
unsigned long int strtoul(const char *nptr, char **endptr, int base) __THROW;

extern int __ltostr(char *s, unsigned int size, unsigned long i, unsigned int base, int UpCase) __THROW;
extern int __dtostr(double d,char *buf,unsigned int maxlen,unsigned int prec,unsigned int prec2,int flags) __THROW;

#if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L
__extension__ long long int strtoll(const char *nptr, char **endptr, int base) __THROW;
__extension__ unsigned long long int strtoull(const char *nptr, char **endptr, int base) __THROW;
__extension__ int __lltostr(char *s, unsigned int size, unsigned long long i, unsigned int base, int UpCase) __THROW;
#endif

int atoi(const char *nptr) __THROW;
long int atol(const char *nptr) __THROW;
double atof(const char *nptr) __THROW;
__extension__ long long int atoll(const char *nptr);

void exit(int status) __THROW __attribute__((__noreturn__));
void abort(void) __THROW __attribute__((__noreturn__));

__attribute__((__warn_unused_result__))
extern int rand(void) __THROW;

__attribute__((__warn_unused_result__))
extern int rand_r(unsigned int *seed) __THROW;
extern void srand(unsigned int seed) __THROW;
#ifdef _BSD_SOURCE
__attribute__((__warn_unused_result__))
extern int random(void) __THROW __attribute_dontuse__;
extern void srandom(unsigned int seed) __THROW __attribute_dontuse__;
#endif

typedef unsigned short randbuf[3];

__attribute__((__warn_unused_result__))
double drand48(void) __THROW;

__attribute__((__warn_unused_result__))
long lrand48(void) __THROW;

__attribute__((__warn_unused_result__))
long mrand48(void) __THROW;

void srand48(long seed) __THROW;
unsigned short *seed48(randbuf buf) __THROW;

void lcong48(unsigned short param[7]) __THROW;

__attribute__((__warn_unused_result__))
long jrand48(randbuf buf) __THROW;

__attribute__((__warn_unused_result__))
long nrand48(randbuf buf) __THROW;

__attribute__((__warn_unused_result__))
double erand48(randbuf buf) __THROW;

__attribute__((__callback(4, 1, 1)))
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
#ifdef _GNU_SOURCE
__attribute__((__callback(4, 1, 1, 5)))
void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void* cookie);
#endif

__attribute__((__callback(5, 1, 2)))
__attribute__((__warn_unused_result__))
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

extern char **environ;

__attribute__((__warn_unused_result__))
char *realpath(const char *path, char *resolved_path) __THROW;

__attribute__((__warn_unused_result__))
int mkstemp(char *_template);

__attribute__((__warn_unused_result__))
char* mkdtemp(char *_template);

__attribute__((__warn_unused_result__))
__attribute_dontuse__
char* mktemp(char *_template);

__attribute__((__warn_unused_result__))
int abs(int i) __THROW __attribute__((__const__));

__attribute__((__warn_unused_result__))
long int labs(long int i) __THROW __attribute__((__const__));
__extension__ long long int llabs(long long int i) __THROW __attribute__((__const__));

#ifdef _XOPEN_SOURCE
__attribute__((__warn_unused_result__))
int grantpt (int fd) __THROW;

__attribute__((__warn_unused_result__))
int unlockpt (int fd) __THROW;

__attribute__((__warn_unused_result__))
char *ptsname (int fd) __THROW;
#endif

#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 	0x7ffffffe

#define MB_CUR_MAX 5

/* now these functions are the greatest bullshit I have ever seen.
 * The ISO people must be out of their minds. */

typedef struct { int quot,rem; } div_t;
typedef struct { long quot,rem; } ldiv_t;

__attribute__((__warn_unused_result__))
div_t div(int numerator, int denominator);

__attribute__((__warn_unused_result__))
ldiv_t ldiv(long numerator, long denominator);

typedef struct { long long quot,rem; } lldiv_t;

__attribute__((__warn_unused_result__))
lldiv_t lldiv(long long numerator, long long denominator);

#ifdef _GNU_SOURCE
int clearenv(void) __THROW;

__attribute__((__warn_unused_result__))
char* secure_getenv(const char* name) __THROW;
#endif

__writemem__(1) __readmemsz__(2,3)
int mbtowc(wchar_t *pwc, const char *s, size_t n) __THROW;
__writemem__(1)
int wctomb(char *s, wchar_t wc) __THROW;
__writemem__(1) __readmemsz__(2,3)
size_t mbstowcs(wchar_t *dest, const char *src, size_t n) __THROW;

__attribute__((__warn_unused_result__))
__readmemsz__(1,2)
int mblen(const char* s,size_t n) __THROW __pure;

__writememsz__(1,3)
size_t wcstombs(char *dest, const wchar_t *src, size_t n) __THROW;

/* These come from OpenBSD: */
__attribute__((__warn_unused_result__))
uint32_t arc4random(void) __THROW;

__writememsz__(1,2)
void arc4random_buf(void* buf, size_t n) __THROW;

__attribute__((__warn_unused_result__))
uint32_t arc4random_uniform(uint32_t upper_bound) __THROW;

__END_DECLS

#endif
