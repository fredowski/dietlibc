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
  if (wcscmp(buf,buf)) die("wcscmp said a != a");
  if (wcscmp(buf,buf2)>=0) die("wcscmp said fnord > fnort");
  if (wcscmp(buf2,buf)<=0) die("wcscmp said fnort < fnord");
  if (wcscmp(buf+1,buf2+1)>=0) die("unaligned wcscmp is broken 1");
  if (wcscmp(buf+2,buf2+2)>=0) die("unaligned wcscmp is broken 2");
  if (wcscmp(buf+3,buf2+3)>=0) die("unaligned wcscmp is broken 3");
  if (wcscmp(L"mäh",L"meh")<0) die("wcscmp uses signed arithmetic");
  if (wcscmp(L"foo",L"foobar")>=0) die("prefix handling broken in wcscmp 1");
  if (wcscmp(L"foobar",L"foo")<=0) die("prefix handling broken in wcscmp 2");
  buf[0]=WCHAR_MIN; buf[1]=0;
  buf2[0]=WCHAR_MAX; buf2[1]=0;
  if (wcscmp(buf,buf2) >= 0) die("wcscmp said WCHAR_MIN > WCHAR_MAX");
  return 0;
}
