#define _GNU_SOURCE
#include <dlfcn.h>

/* Have a dummy version of libdl for static linking that will just
 * always fail immediately when used. */

#ifndef __DYN_LIB

__attribute__((weak)) void* dlopen(const char* filename, int flag) { (void)filename; (void)flag; return 0; }
__attribute__((weak)) const char* dlerror(void) { return ""; }
__attribute__((weak)) void* dlsym(void* handle, const char* symbol) { (void)handle; (void)symbol; return 0; }
__attribute__((weak)) int dlclose(void* handle) { (void)handle; return 0; }

__attribute__((weak)) int dladdr(void *addr, Dl_info *info) { (void)addr; (void)info; return 0; }

#endif
