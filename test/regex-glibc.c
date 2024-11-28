/***********************************************************

Copied from glibc, adapted - 2024 - Friedrich Beckmann

Copyright 1995 by Tom Lord

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the copyright holder not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

Tom Lord DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL TOM LORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define IGNORE_OUTOFRANGE 1

struct a_test
{
  int expected; /* 0: Compiles, finds matches, 1: No matches found, 2: Compile error*/
  const char * pattern;
  const char * data;
  const char *matches[10];
};

static const struct a_test the_tests[] =
{
#include "regex-glibc-testcases.h"
  {0, "ab((c*)d)ef", "abcdefg", { "abcdef", "cd", "c", NULL } },
  {-1, 0, 0}
};

int
regmatch_out_of_range (regmatch_t reg, size_t len)
{
  return (reg.rm_so < 0 || reg.rm_so > len ||
          reg.rm_eo < 0 || reg.rm_eo > len ||
          reg.rm_eo < reg.rm_so);
}

static int
run_a_test (int id, const struct a_test * t)
{
  static const char * last_pattern = 0;
  static regex_t r;
  int err;
  char errmsg[100];
  int x;
  regmatch_t regs[10];
  int match_error = false;
  size_t len = strlen(t->data);

  if (!last_pattern || strcmp (last_pattern, t->pattern))
    {
      if (last_pattern)
	regfree (&r);
      last_pattern = t->pattern;
      err = regcomp (&r, t->pattern, REG_EXTENDED);
      if (err)
	{
	  if (t->expected == 2)
	    {
	      puts (" OK.");
	      return 0;
	    }
	  if (last_pattern)
	    regfree (&r);
	  last_pattern = NULL;
	  regerror (err, &r, errmsg, 100);
	  printf (" FAIL: %s.\n", errmsg);
	  return 1;
	}
      else if (t->expected == 2)
	{
	  printf ("test %d\n", id);
	  printf ("pattern \"%s\" successful compilation not expected\n",
		  t->pattern);
	  return 1;
	}
    }

  err = regexec (&r, t->data, 10, regs, 0);

  if (err == 0) { // dut thinks we have a match
    int no_of_matches = 0;
    for (x = 0;x < 10; ++x) {
      if (regs[x].rm_so == -1) {
        if (t->matches[x] == (const char *)NULL)
          continue;
        else {
          match_error = true;
          break;
        }
      } else {
#if IGNORE_OUTOFRANGE
        if ( t->matches[x] == (const char *)NULL )
          continue;
#endif
        if ( (! regmatch_out_of_range(regs[x], len)) &&
             ( t->matches[x] != (const char *)NULL ) &&
             strncmp(t->data + (size_t) regs[x].rm_so,
                     t->matches[x],
                     (size_t) (regs[x].rm_eo - regs[x].rm_so)) == 0 ) {
          no_of_matches++;
          continue;
        } else {
          match_error = true;
          break;
        }
      }
    }
    if (no_of_matches == 0)
      match_error = true;
  }

  if ((err == 0 && t->expected != 0) ||
      (err != 0 && t->expected == 0) ||
      match_error)
    {
      printf ("pattern \"%s\" data \"%s\" wanted %d got %d\n",
	      t->pattern, t->data, t->expected, err);
      if (err == 0) {
        size_t len = strlen(t->data);
        for (x = 0; x < 10; ++x) {
          if (regs[x].rm_so == -1)
            printf("reg %d == (-1, U) act: no match - exp: %s\n",
                x,
                (t->matches[x] == (const char *)NULL ? "no match" : t->matches[x]));
          else if (regmatch_out_of_range(regs[x], len)) {
            printf("reg %d == (%ld, %ld) exp: %s\n",
              x,
              regs[x].rm_so,
              regs[x].rm_eo,
              (t->matches[x] == (const char *)NULL ? "no match" : t->matches[x]));
          } else {
            printf ("reg %d == (%ld, %ld) act: %.*s exp: %s\n",
		x,
		regs[x].rm_so,
		regs[x].rm_eo,
		(int) (regs[x].rm_eo - regs[x].rm_so),
		t->data + (size_t) regs[x].rm_so,
                (t->matches[x] == (const char *)NULL ? "no match": t->matches[x]));
          }
        }
      }
      return 1;
    }
  puts (" OK.");
  return 0;
}

int
main (int argc, char * argv[])
{
  int x;
  int lo;
  int hi;
  int res = 0;

  lo = 0;
  hi = (sizeof (the_tests) / sizeof (the_tests[0])) - 1;

  if (argc > 1)
    {
      lo = atoi (argv[1]);
      hi = lo + 1;

      if (argc > 2)
	hi = atoi (argv[2]);
    }

  for (x = lo; x < hi; ++x)
    {
      printf ("#%d:", x);
      res |= run_a_test (x, &the_tests[x]);
    }
  return res != 0;
}
