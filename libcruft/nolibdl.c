#include <dlfcn.h>

/* Have a dummy version of libdl for static linking that will just
 * always fail immediately when used. */

__attribute__((weak)) void* dlopen(const char* filename, int flag) { return 0; }
__attribute__((weak)) const char* dlerror(void) { return ""; }
__attribute__((weak)) void* dlsym(void* handle, const char* symbol) { return 0; }
__attribute__((weak)) int dlclose(void* handle) { return 0; }

