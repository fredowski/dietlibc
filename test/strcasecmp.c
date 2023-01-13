#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void die(const char* message) {
  puts(message);
  exit(1);
}

int main() {
  char buf[100]="fNord";
  char buf2[100]="fnOrt";
  if (strcasecmp(buf,buf)) die("strcasecmp said a != a");
  if (strcasecmp(buf,buf2)>=0) die("strcasecmp said fnord > fnort");
  if (strcasecmp(buf2,buf)<=0) die("strcasecmp said fnort < fnord");
  if (strcasecmp("Host","hostbasedauthentication")==0) die("strcasecmp said a == abc");
  if (strcasecmp("\001","\377") >= 0) die("strcasecmp said 1 >= 255");
  if (strcasecmp("\x7f","\x80") >= 0) die("strcasecmp said 127 >= -128");
  return 0;
}
