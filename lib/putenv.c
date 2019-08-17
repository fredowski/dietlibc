#include <stdlib.h>
#include <string.h>
#include <errno.h>

int putenv(const char *string) {
  size_t len;
  size_t i, envc;
  int remove=0;
  char *tmp;
  const char **ep;
  char **newenv;
  static char **origenv;
  if (!origenv) origenv=environ;

  // first find out of we are asked to remove or add/change an entry
  // putenv("FOO") means remove all FOO=* entries.
  if (!(tmp=strchr(string,'='))) {
    len=strlen(string);
    remove=1;
  } else
    len=tmp-string;

  // now loop through the environment looking for the string
  ep=(const char**)environ;
  if (!ep) return 0;
  for (i=envc=0, ep[i]; ep[i]; ++i) {
    // i is the iterator for reading, envc for writing
    // we compare the first byte before calling memcmp as an optimization
    if (*string == ep[i][0] && !memcmp(string,ep[i],len) && ep[i][len]=='=') {
      // we found the prefix
      // if we were asked to remove said entry, we are done
      if (remove) continue;
      // if we were asked to replace, do that, and for future matches
      // pretend we were asked to remove
      ep[envc++]=string;
      remove=1;
    } else
      ep[envc++]=ep[i];
  }
  if (remove==0) {
    // we were asked to replace something that wasn't there
    // so realloc and add here
    newenv = (char**) realloc(environ==origenv?0:environ,
			      (envc+2)*sizeof(char*));
    if (!newenv) return -1;
    if (envc && (environ==origenv)) {
      memcpy(newenv,origenv,envc*sizeof(char*));
    }
    newenv[envc++]=(char*)string;
    environ=newenv;
  }
  environ[envc]=0;		// terminate env
  return 0;
}

#ifdef UNITTEST
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int
main ()
{
  char* a="FOO=bar";
  char* b="FOO=baz";
  char* c="FOO=blub";
  char* fraud[]={a, b, 0};
  environ=fraud;

  // test fraudulent environments
  assert(putenv(c) == 0);	// invalid env: FOO appears twice
  assert(environ[0]==c && environ[1]==0);

  putenv("NEWVAR1=NEWVAL1");
  putenv("NEWVAR2=NEWVAL2");

  assert (!strcmp(getenv("NEWVAR1"), "NEWVAL1"));

  // now do some more basic tests
  static char foo[] = "Hello=World";
  assert (putenv ("foo=bar") != -1);
  assert (!strcmp (getenv ("foo"), "bar"));
  assert (putenv ("foo=baz") != -1);
  assert (!strcmp (getenv ("foo"), "baz"));
  putenv (foo);
  assert (!strcmp (getenv ("Hello"), "World"));
  foo[6] = 'M';
  assert (!strcmp (getenv ("Hello"), "Morld"));

  return 0;
}
#endif
