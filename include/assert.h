#ifndef _ASSERT_H
#define _ASSERT_H

#include <sys/cdefs.h>

/* This prints an "Assertion failed" message and aborts.  */
extern void __assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function)
     __THROW __attribute__ ((__noreturn__));

#ifdef __PRETTY_FUNCTION__
#define __ASSERT_FUNCTION __PRETTY_FUNCTION__
#else
#define __ASSERT_FUNCTION __func__
#endif

#undef assert
#ifdef NDEBUG
#define assert(expr)
#else
# define assert(expr)							      \
  ((void) ((expr) ? 0 :							      \
	   (__assert_fail (#expr,				      \
			   __FILE__, __LINE__, __ASSERT_FUNCTION), 0)))
#endif

#endif
