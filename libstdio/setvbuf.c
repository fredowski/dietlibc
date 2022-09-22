#include <sys/types.h>
#include "dietstdio.h"
#include <unistd.h>
#include <stdlib.h>
#include "dietwarning.h"

static int set_flags(FILE *stream, int flags) {
  switch (flags) {
    case _IONBF: stream->flags = (stream->flags & ~(BUFLINEWISE)) | NOBUF; break;
    case _IOLBF: stream->flags = (stream->flags & ~(NOBUF)) | BUFLINEWISE; break;
    case _IOFBF: stream->flags = stream->flags & ~(NOBUF | BUFLINEWISE); break;
    default: return -1;
  }
  return 0;
}

int setvbuf_unlocked(FILE *stream, char *buf, int flags, size_t size) {
  if (buf) {
    /* don't leak contents of file in case it was a private key */
    explicit_memset(stream->buf, 0, stream->buflen);

    if (!(stream->flags&STATICBUF)) free(stream->buf);
    stream->buf=buf;
  }
  else {
    char *tmp;
    if (!size) {
      return set_flags(stream,flags);
    }
    if (!(tmp=malloc(size))) return -1;

    /* don't leak contents of file in case it was a private key */
    explicit_memset(stream->buf, 0, stream->buflen);

    if (!(stream->flags&STATICBUF)) free(stream->buf);
    stream->buf=tmp;
  }
  stream->flags &= ~STATICBUF;
  stream->buflen=size;
  stream->bm=stream->bs=0;
  return set_flags(stream,flags);
}

int setvbuf(FILE *stream, char *buf, int flags, size_t size) __attribute__((weak,alias("setvbuf_unlocked")));
