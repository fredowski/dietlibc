#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>

int main() {
  wchar_t* p=L"/opt/diet/bin:/home/leitner/bin:/usr/local/bin:/opt/cross/bin:/usr/local/sbin:/usr/bin:/sbin:/bin:/usr/sbin:/usr/X11R6/bin:/opt/teTeX/bin:/opt/qt-4.3.2/bin:/opt/kde-3.5/bin:/usr/X11R7/bin:/opt/mono/bin";
  assert(wcschr(p,':')==p+13);
  assert(wcschr(p=L"fnord",'\0')==p+5);
  assert(wcschr(p,'r')==p+3);
  assert(wcschr(p,'x')==0);
  return 0;
}
