#include <wchar.h>

size_t wcsnlen(const wchar_t* s,size_t maxl) {
  size_t i;
  for (i=0; i<maxl && s[i]; ++i) ;
  return i;
}
