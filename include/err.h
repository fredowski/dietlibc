#ifndef _ERR_H
#define _ERR_H

#include <sys/cdefs.h>
#include <stdarg.h>

void err(int s, const char *f, ...) __attribute__((__deprecated__,noreturn,format(printf,2,3)));
void errc(int s, int e, const char *f, ...) __attribute__((__deprecated__,noreturn,format(printf,3,4)));
void errx(int s, const char *f, ...) __attribute__((__deprecated__,noreturn,format(printf,2,3)));

void warn(const char *f, ...) __attribute__((__deprecated__,format(printf,1,2)));
void warnc(int e, const char *f, ...) __attribute__((__deprecated__,format(printf,2,3)));
void warnx(const char *f, ...) __attribute__((__deprecated__,format(printf,1,2)));

void verr(int s, const char *f, va_list va) __attribute__((__deprecated__,noreturn,format(printf,2,0)));
void verrc(int s, int e, const char *f, va_list va) __attribute__((__deprecated__,noreturn,format(printf,3,0)));
void verrx(int s, const char *f, va_list va) __attribute__((__deprecated__,noreturn,format(printf,2,0)));

void vwarn(const char *f, va_list va) __attribute__((__deprecated__,format(printf,1,0)));
void vwarnc(int e, const char *f, va_list va) __attribute__((__deprecated__,format(printf,2,0)));
void vwarnx(const char *f, va_list va) __attribute__((__deprecated__,format(printf,1,0)));

#endif
