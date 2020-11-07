#ifndef _STRINGS_H
#define _STRINGS_H

#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

int strcasecmp(const char *s1, const char *s2) __THROW __pure;
int strncasecmp(const char *s1, const char *s2, size_t n) __THROW __pure;
int ffs(int i) __THROW __attribute__((__const__));

__readmemsz__(1,3) __readmemsz__(2,3)
int    bcmp(const void *, const void *, size_t) __THROW __pure __attribute_dontuse__;

__writememsz__(2,3) __readmemsz__(1,3)
void   bcopy(const void *, void *, size_t) __THROW __attribute_dontuse__;

__writememsz__(1,2)
void   bzero(void *, size_t) __THROW __attribute_dontuse__;

__readmem__(1)
char  *index(const char *, int) __THROW __pure __attribute_dontuse__;

__readmem__(1)
char  *rindex(const char *, int) __THROW __pure __attribute_dontuse__;

#define bzero(s,n) memset(s,0,n)
#define bcopy(src,dest,n) memmove(dest,src,n)
#define bcmp(a,b,n) memcmp(a,b,n)
#define index(a,b) strchr(a,b)
#define rindex(a,b) strrchr(a,b)

__END_DECLS

#endif
