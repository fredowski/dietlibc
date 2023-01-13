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
  if (strncasecmp(buf,buf,100)) die("strcasecmp said a != a");
  if (strncasecmp(buf,buf2,100)>=0) die("strcasecmp said fnord > fnort");
  if (strncasecmp(buf2,buf,100)<=0) die("strcasecmp said fnort < fnord");
  if (strncasecmp("Host","hostbasedauthentication",100)==0) die("strcasecmp said a == abc");
  if (strncasecmp("\001","\377",100) >= 0) die("strcasecmp said 1 >= 255");

  if (strncasecmp(buf,buf2,4) != 0) die("strcasecmp didn't stop after n chars");
  return 0;
}
