#include <sys/types.h>
#include "dietstdio.h"
#include <unistd.h>

size_t fread( void *ptr, size_t size, size_t nmemb, FILE *stream) {
  int res;
  unsigned long i,j;
  j=size*nmemb;
  i=0;

  if (!j || j/nmemb!=size) return 0;
  if (stream->ungotten) {
    stream->ungotten=0;
    *(char*)ptr=stream->ungetbuf;
    ++i;
  }
  if (!j) return 1;

#ifdef WANT_FREAD_OPTIMIZATION
  if (j>stream->buflen) {
    size_t tmp=j-i;
    int res;
    fflush(stream);
    while ((res=read(stream->fd,ptr+i,tmp))<(int)tmp) {
      if (res==-1) {
	stream->flags|=ERRORINDICATOR;
	goto exit;
      } else if (!res) {
	stream->flags|=EOFINDICATOR;
	goto exit;
      }
      i+=res; tmp-=res;
    }
    return nmemb;
  }
#endif
  for (; i<j; ++i) {
    res=fgetc(stream);
    if (res==EOF)
exit:
      return i/size;
    else
      ((unsigned char*)ptr)[i]=(unsigned char)res;
  }
  return nmemb;
}
