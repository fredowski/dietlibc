#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdint.h>

void die(const char* message) {
  puts(message);
  exit(1);
}

int main() {
  wchar_t buf[100]=L"fnord";
  wchar_t buf2[100]=L"fnort";
  if (wcsncmp(buf,buf,100)) die("wcsncmp said a != a");
  if (wcsncmp(buf,buf2,100)>=0) die("wcsncmp said fnord > fnort");
  if (wcsncmp(buf2,buf,100)<=0) die("wcsncmp said fnort < fnord");
  if (wcsncmp(buf+1,buf2+1,99)>=0) die("unaligned wcsncmp is broken 1");
  if (wcsncmp(buf+2,buf2+2,98)>=0) die("unaligned wcsncmp is broken 2");
  if (wcsncmp(buf+3,buf2+3,97)>=0) die("unaligned wcsncmp is broken 3");
  if (wcsncmp(L"mäh",L"meh",4)<0) die("wcsncmp uses signed arithmetic");
  if (wcsncmp(L"foo",L"foobar",7)>=0) die("prefix handling broken in wcsncmp 1");
  if (wcsncmp(L"foobar",L"foo",7)<=0) die("prefix handling broken in wcsncmp 2");

  if (wcsncmp(buf,buf2,4) != 0) die("wcsncmp did not stop after n wchars");

  buf[0]=WCHAR_MIN; buf[1]=0;
  buf2[0]=WCHAR_MAX; buf2[1]=0;
  if (wcsncmp(buf,buf2,1) >= 0) die("wcsncmp said WCHAR_MIN > WCHAR_MAX");

  return 0;
}
