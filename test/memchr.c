#include <string.h>
#include <assert.h>

int main() {
  const char* test="blubber";
  assert(memchr("aaaa",'x',4)==0);
  assert(memchr(0,'x',0)==0);
  assert(memchr(test,'u',2) == NULL);
  assert(memchr(test,'u',7) == test+2);
  assert(memchr(test,'b',7)==test);
  assert(memchr(test+6,'r',1)==test+6);
  assert(memchr(test+7,0,100)==test+7);

  const char* test2="\x01\x02\x00\x00\x00";
  assert(memchr(test2, 258, 5) == test2+1);
  return 0;
}
