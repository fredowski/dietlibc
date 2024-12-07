#define NDEBUG
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#ifdef DEBUG
#include <stdio.h>
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(a,...) {}
#endif

#if !defined(__x86_64__)
#undef WANT_REGEX_JIT
#endif

/* this is ugly.
 * the idea is to build a parse tree, then do some poor man's OOP with a
 * generic matcher function call that is always that the start of each
 * record, and a next pointer.  When the parse tree is done, we need to
 * recursively set the next pointers to point to the part of the parse
 * tree that needs to match next.
 * This is the prototype of the generic match function call pointer.
 * The first argument is the "this" pointer, the second is the text to
 * be matched against, ofs is the offset from the start of the matched
 * text (so we can match "^") and matches is an array where match
 * positions are stored. */
/* now declared in regex.h: */
/* typedef int (*matcher)(void*,const char*,int ofs,regmatch_t* matches,int plus,int eflags); */

/* one would think that this is approach is an order of magnitude slower
 * than the standard NFA approach, but it isn't.  The busybox grep took
 * 0.26 seconds for a fixed string compared to 0.19 seconds for the
 * glibc regex. */

/* first part: parse a regex into a parse tree */
struct bracketed {
  unsigned int cc[32];
};

/* now declared in regex.h:
struct regex {
  matcher m;
  void* next;
  int pieces;
  int num;
  struct branch *b;
}; */

struct string {
  char* s;
  size_t len;
};

struct atom {
  matcher m;
  void* next;
  int startofs;
  enum { ILLEGAL, EMPTY, REGEX, BRACKET, ANY, LINESTART, LINEEND, WORDSTART, WORDEND, CHAR, STRING, BACKREF, } type;
  int bnum;
  union {
    struct regex r;
    struct bracketed b;
    char c;
    struct string s;
  } u;
};

struct piece {
  matcher m;
  void* next;
  int startofs;
  struct atom a;
  unsigned int min,max;
};

struct branch {
  matcher m;
  void* next;
  int startofs;
  int num;
  struct piece *p;
};

static void clearcc(unsigned int* x) {
  memset(x,0,sizeof(struct bracketed));
}

static void setcc(unsigned int* x,unsigned int bit) {
  x[bit/32]|=(1<<((bit%32)-1));
}

static int issetcc(unsigned int* x,unsigned int bit) {
  return x[bit/32] & (1<<((bit%32)-1));
}

static const char* parsebracketed(struct bracketed*__restrict__ b,const char*__restrict__ s,regex_t*__restrict__ rx) {
  const char* t;
  int i,negflag=0;
  dprintf("parsebracketed %c ", *(s));
  if (*s!='[') return s;
  t=s+1;
  clearcc(b->cc);
  if (*t=='^') { negflag=1; ++t; }
  do {
    if (*t==0) {
        dprintf("no bracketed found\n");
        return s;
    }
    setcc(b->cc,rx->cflags&REG_ICASE?tolower(*t):*t);
    if (t[1]=='-' && t[2]!=']') {
      for (i=*t+1; i<=t[2]; ++i) setcc(b->cc,rx->cflags&REG_ICASE?tolower(i):i);
      t+=2;
    }
    ++t;
  } while (*t!=']');
  if (negflag) for (i=0; i<32; ++i) b->cc[i]=~b->cc[i];
  dprintf("parsebracketed found bracket, continue with %c\n", *(t+1));
  return t+1;
}

static const char* parseregex(struct regex* r,const char* s,regex_t* rx);

static int matchatom_CHAR(void*__restrict__ x,const unsigned char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  register struct atom* a=(struct atom*)x;
#ifdef DEBUG
    printf("matching atom CHAR %c against \"%.20s\"\n",a->u.c,s);
#endif
  a->startofs=ofs;
  if (*s!=a->u.c) return -1;
  if (a->next)
    return ((struct atom*)(a->next))->m(a->next,(const char*)s+1,ofs+1,preg,plus+1,eflags);
  else
    return plus+1;
}

static int matchatom_CHAR_ICASE(void*__restrict__ x,const unsigned char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  register struct atom* a=(struct atom*)x;
#ifdef DEBUG
    printf("matching atom CHAR_ICASE %c against \"%.20s\"\n",a->u.c,s);
#endif
  a->startofs=ofs;
  if (tolower(*s)!=a->u.c) return -1;
  if (a->next)
    return ((struct atom*)(a->next))->m(a->next,(const char*)s+1,ofs+1,preg,plus+1,eflags);
  else
    return plus+1;
}

static int matchatom(void*__restrict__ x,const unsigned char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  register struct atom* a=(struct atom*)x;
  int matchlen=0;
  assert(a->type!=ILLEGAL);
  a->startofs=ofs;
  switch (a->type) {
  case EMPTY:
#ifdef DEBUG
    printf("matching atom EMPTY against \"%.20s\"\n",s);
    printf("a->bnum is %d\n",a->bnum);
#endif
    if (a->bnum>0) {
      preg->l[a->bnum].rm_so=preg->l[a->bnum].rm_eo=ofs;
      dprintf("matchatom: bnum: %d -> rm_so=%d, rm_eo=%d",a->bnum,ofs,ofs);
    }
    goto match;
  case REGEX:
#ifdef DEBUG
    printf("matching atom (%p) REGEX against \"%.20s\"\n",a,s);
    printf("a->bnum is %d\n",a->bnum);
#endif
    /* If there are further BACKREFs we need to fill the match register.
       So we do a first run and fill the match register on speculation. */
    if (((preg->cflags&REG_EXTENDED)==0) && a->bnum>0) {
        matchlen=a->u.r.m(&a->u.r,(const char*)s,ofs,preg,0,eflags);
        preg->l[a->bnum].rm_so=ofs;
        preg->l[a->bnum].rm_eo=(a->next?((struct atom *)(a->next))->startofs:ofs+matchlen);
    }
    if ((matchlen=a->u.r.m(&a->u.r,(const char*)s,ofs,preg,0,eflags))>=0) {
      int eo=(a->next?((struct atom *)(a->next))->startofs:ofs+matchlen);
      assert(a->bnum>=0);
      preg->l[a->bnum].rm_so=ofs;
      preg->l[a->bnum].rm_eo=eo;
      dprintf("matchatom: bnum: %d -> rm_so=%d, rm_eo=%d, next: %p\n",a->bnum,ofs,eo,a->next);
      return plus+matchlen;
    } else if (a->bnum>0) {
      dprintf("matchatom: bnum: %d -> rm_so=%d, rm_eo=%d\n",a->bnum,-1,-1);
      preg->l[a->bnum].rm_so=-1;
    }
    break;
  case BRACKET:
#ifdef DEBUG
    printf("matching atom BRACKET against \"%.20s\"\n",s);
#endif
    matchlen=1;
    if (*s=='\n' && (preg->cflags&REG_NEWLINE)) break;
    if (*s && issetcc(a->u.b.cc,(preg->cflags&REG_ICASE?tolower(*s):*s)))
      goto match;
    break;
  case ANY:
#ifdef DEBUG
    printf("matching atom ANY against \"%.20s\"\n",s);
#endif
    if (*s=='\n' && (preg->cflags&REG_NEWLINE)) break;
    matchlen=1;
    if (*s) goto match;
    break;
  case LINESTART:
#ifdef DEBUG
    printf("matching atom LINESTART against \"%.20s\"\n",s);
#endif
    if ((eflags&REG_NOTBOL)==0)
      if (ofs==0 || ((preg->cflags&REG_NEWLINE) && s[-1]=='\n'))
	goto match;
    break;
  case LINEEND:
#ifdef DEBUG
    printf("matching atom LINEEND against \"%.20s\"\n",s);
#endif
    if ((preg->cflags&REG_NEWLINE) && *s=='\n') goto match;
    if ((*s && *s!='\n') || (eflags&REG_NOTEOL)) break;
    goto match;
  case WORDSTART:
#ifdef DEBUG
    printf("matching atom WORDSTART against \"%.20s\"\n",s);
#endif
    if ((ofs==0 || !isalnum(s[-1])) && isalnum(*s))
      goto match;
    break;
  case WORDEND:
#ifdef DEBUG
    printf("matching atom WORDEND against \"%.20s\"\n",s);
#endif
    if (ofs>0 && isalnum(s[-1]) && !isalnum(*s))
      goto match;
    break;
  case CHAR:
#ifdef DEBUG
    printf("matching atom (%p) CHAR %c against \"%.20s\"\n",a,a->u.c,s);
#endif
    matchlen=1;
    if (((preg->cflags&REG_ICASE)?tolower(*s):*s)==a->u.c) goto match;
    break;
  case STRING:
    matchlen=a->u.s.len;
#ifdef DEBUG
    printf("matching atom STRING \"%.*s\" against \"%.20s\"\n",(int)a->u.s.len,a->u.s.s,s);
#endif
    {
      int i;
      if (preg->cflags&REG_ICASE) {
	for (i=0; i<matchlen; ++i)
	  if (tolower(s[i]) != a->u.s.s[i]) return -1;
      } else {
	for (i=0; i<matchlen; ++i)
	  if (s[i] != a->u.s.s[i]) return -1;
      }
    }
    goto match;
    break;
  case BACKREF:
    matchlen=preg->l[(unsigned char)(a->u.c)].rm_eo-preg->l[(unsigned char)(a->u.c)].rm_so;
#ifdef DEBUG
    printf("matching atom BACKREF %d (\"%.*s\") against \"%.20s\"\n",(int)a->u.c,matchlen,s-ofs+preg->l[(size_t)(a->u.c)].rm_so,s);
#endif
    if (memcmp(s-ofs+preg->l[(unsigned char)(a->u.c)].rm_so,s,matchlen)==0) goto match;
    break;
  }
  dprintf("matchatom: no match, return -1\n");
  return -1;
match:
  if (a->next)
    return ((struct atom*)(a->next))->m(a->next,(const char*)s+matchlen,ofs+matchlen,preg,plus+matchlen,eflags);
  else
    return plus+matchlen;
}

static int closebracket(const char* s,const regex_t* r) {
  if (r->cflags&REG_EXTENDED)
    return *s==')';
  else
    return (*s=='\\' && s[1]==')');
}

static const char* parseatom(struct atom*__restrict__ a,const char*__restrict__ s,regex_t*__restrict__ rx) {
  const char *tmp;
  a->m=(matcher)matchatom;
  a->bnum=-1;
  dprintf("parseatom: %c\n", *s);
  switch (*s) {
  case '(':
    if ((rx->cflags&REG_EXTENDED)==0) goto handle_char;
openbracket:
    a->bnum=++rx->brackets;
    if (s[1]==')') {
      a->type=EMPTY;
      dprintf("parseatom: found empty regex, continue with %c\n", *(s+2));
      return s+2;
    }
    a->type=REGEX;
    tmp=parseregex(&a->u.r,s+1,rx);
    if (closebracket(tmp,rx)) {
      dprintf("parseatom: found regex, continue with %c\n", *(tmp+1));
      return tmp+1+((rx->cflags&REG_EXTENDED)==0);
    }
    /* fall through */
  case ')':
    if ((rx->cflags&REG_EXTENDED)==0) goto handle_char;
    /* fall through */
  case 0:
  case '|':
    dprintf("parseatom: found ) or | or null, continue with %c\n", *s);
    return s;
  case '[':
    a->type=BRACKET;
    if ((tmp=parsebracketed(&a->u.b,s,rx))!=s) {
      dprintf("parseatom: found bracket, continue with %c\n", *tmp);
      return tmp;
    }
    dprintf("parseatom: bracket parse no success, continue with %c\n", *s);
    return s;
  case '.':
    a->type=ANY;
    break;
  case '^':
    a->type=LINESTART;
    break;
  case '$':
    a->type=LINEEND;
    break;
  case '\\':
    if (!*++s) return s;
    if (*s=='<') {
      a->type=WORDSTART;
      break;
    } else if (*s=='>') {
      a->type=WORDEND;
      break;
    } else if (*s>='1' && *s<=(rx->brackets+'1') && ((rx->cflags&REG_EXTENDED)==0)) {
      dprintf("parseatom: found BACKREF\n");
      a->type=BACKREF;
      a->u.c=*s-'0';
      break;
    } else if ((rx->cflags&REG_EXTENDED)==0) {
      if (*s=='(') goto openbracket; else
      if (*s==')') return s-1;
    }
    /* fall through */
  default:
handle_char:
    a->type=CHAR;
    if (rx->cflags&REG_ICASE) {
      a->u.c=tolower(*s);
      a->m=(matcher)matchatom_CHAR_ICASE;
    } else {
      a->u.c=*s;
      a->m=(matcher)matchatom_CHAR;
    }
    /* optimization: if we have a run of CHAR, make it into a STRING */
    {
      size_t i;
      for (i=1; s[i] && !strchr("(|)[.^$\\*+?{",s[i]); ++i) ;
      if (strchr("*+?{",s[i])) --i;
      if (i>2) {
	a->m=(matcher)matchatom;
	a->type=STRING;
	a->u.s.len=i;
	if (!(a->u.s.s=malloc(i+1))) return s;
	memcpy(a->u.s.s,s,i);
	a->u.s.s[i]=0;
	dprintf("parseatom: continue with %c\n", *(s+1));
	return s+i;
      }
    }
    break;
  }
  dprintf("parseatom: continue with %c\n", *(s+1));
  return s+1;
}

static void atom_putnext(struct atom*__restrict__ a,void*__restrict__ next);

/* needs to do "greedy" matching, i.e. match as often as possible */
static int matchpiece(void*__restrict__ x,const char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  register struct piece* a=(struct piece*)x;
  int matchlen=0;
  int tmp=0,num=0;
  unsigned int *offsets;
  void* save = 0;
  assert(a->max>0 && a->max<=RE_DUP_MAX);
  a->startofs=ofs;
#ifdef DEBUG
  printf("matchpiece (%p) \"%s\" min:%d max:%d\n", x, s, a->min, a->max);
  //printf("alloca(%d)\n",(int)sizeof(int)*(a->max+1));
#endif
  offsets=alloca(sizeof(int)*(a->max+1));
  offsets[0]=0;
  //dprintf("allocating %d offsets...\n",a->max);
  //dprintf("matchpiece \"%s\"...\n",s);
  /* first, try to match the atom as often as possible, up to a->max times */
  if (a->max == 1 && a->min == 1) {
    tmp = a->a.m(&a->a,s+matchlen,ofs+matchlen,preg,plus,eflags);
    dprintf("matchpiece (%p) return %d\n", x,tmp);
    return tmp;
  }
  save=a->a.next;
  atom_putnext(&a->a,0);
  while (num<(int)a->max) {
    tmp=a->a.m(&a->a,s+matchlen,ofs+matchlen,preg,0,eflags);
    if (tmp>=0) {
      ++num;
      matchlen+=tmp;
      dprintf("matchpiece (%p) setting offsets[%d] to %d\n",x, num,tmp);
      offsets[num]=tmp;
      if (tmp==0) break;
    } else {
      break;
    }
  }
  atom_putnext(&a->a,save);
  dprintf("matchpiece (%p): working backwards with num=%d\n",x,num);
  if (num<(int)a->min) return -1;		/* already at minimum matches; signal mismatch */
  /* then, while the rest does not match, back off */
  for (;num>=(int)a->min;) {
    if (a->next) {
      dprintf("matchpiece (%p): trying next\n",x);
      tmp=((struct atom*)(a->next))->m(a->next,s+matchlen,ofs+matchlen,preg,plus+matchlen,eflags);
    } else {
      dprintf("matchpiece (%p): a->next is NULL\n",x);
      tmp=plus+matchlen;
    }
    dprintf("matchpiece (%p): tmp=%d\n",x,tmp);
    if (tmp>=0) break;	/* it did match; don't back off any further */
    dprintf("matchpiece (%p): using offsets[%d] (%d)\n",x,num,offsets[num]);
    matchlen-=offsets[num];
    --num;
  }
  /* Now run the matcher again from the last successfull offset to fill the
     match registers of possible regex atoms */
  if (tmp>=0) {
    dprintf("matchpiece (%p): final correction run\n",x);
    matchlen-=offsets[num];
    a->a.m(&a->a,s+matchlen,ofs+matchlen,preg,plus,eflags);
  }
  dprintf("matchpiece (%p): returning tmp=%d\n",x,tmp);
  return tmp;
}

static const char* parsepiece(struct piece*__restrict__ p,const char*__restrict__ s,regex_t*__restrict__ rx) {
  dprintf("parsepiece %c\n", *s);
  const char* tmp=parseatom(&p->a,s,rx);
  if (tmp==s) {
        dprintf("parsepiece found nothing\n");
        return s;
  }
  p->m=matchpiece;
  p->min=p->max=1;
  switch (*tmp) {
  case '*': p->min=0; p->max=RE_DUP_MAX; break;
  case '+': p->min=1; p->max=RE_DUP_MAX; break;
  case '?': p->min=0; p->max=1; break;
  case '{':
    if (isdigit(*++tmp)) {
      p->min=*tmp-'0'; p->max=RE_DUP_MAX;
      while (isdigit(*++tmp)) p->min=p->min*10+*tmp-'0';
      if (*tmp==',') {
	if (isdigit(*++tmp)) {
	  p->max=*tmp-'0';
	  while (isdigit(*++tmp)) p->max=p->max*10+*tmp-'0';
	}
      } else
	p->max=p->min;
      if (*tmp!='}') return s;
      ++tmp;
    }
    /* fall through */
  default:
    dprintf("parsepiece found piece, continue with %c\n",*tmp);
    return tmp;
  }
  dprintf("parsepiece found piece, continue with %c\n",*(tmp+1));
  return tmp+1;
}

/* trivial, just pass through */
static int matchbranch(void*__restrict__ x,const char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  register struct branch* a=(struct branch*)x;
  int tmp;
#ifdef DEBUG
  printf("%p matching branch against \"%.20s\"\n",a,s);
  printf("%p %p\n",&a->p->m,a->p->m);
#endif
  a->startofs=ofs;
  assert(a->p->m==matchpiece);
  tmp=a->p->m(a->p,s,ofs,preg,plus,eflags);
  dprintf("matchbranch (%p) return %d\n",a,tmp);
  return tmp;
}

static int matchempty(void*__restrict__ x,const char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  struct atom *a = (struct atom *)x;
  if (a->next)
    return ((struct atom*)(a->next))->m(a->next,(const char*)s,ofs,preg,plus,eflags);
  else
    return plus;
}

static const char* parsebranch(struct branch*__restrict__ b,const char*__restrict__ s,regex_t*__restrict__ rx,int*__restrict__ pieces) {
  struct piece p;
  const char *tmp = NULL;
  dprintf("parsebranch: %c\n",*s);
  b->m=matchbranch;
  b->num=0; b->p=0;
  for (;;) {
    if (*s=='|' && b->num==0) {
      tmp=s;
      p.a.type=EMPTY;
      p.a.m=matchempty;
      p.min=p.max=1;
      p.m=matchpiece;
    } else {
      tmp=parsepiece(&p,s,rx);
      if (tmp==s) return s;
    }
    //printf("parsebranch: b->p from %p to ",b->p);
    {
      struct piece* t;
      if (!(t=realloc(b->p,++b->num*sizeof(p)))) return s;
      //printf("parsebranch: piece realloc: %p -> %p (%d*%d)\n",b->p,t,b->num,(int)sizeof(p));
      b->p=t;
    }
    //printf("%p (size %d)\n",b->p,(int)(b->num*sizeof(p)));
    b->p[b->num-1]=p;
    dprintf("assigned piece %d in branch %p\n",b->num-1,b);
    if (*tmp=='|') break;
    s=tmp;
  }
  *pieces+=b->num;
  return tmp;
}

/* try the branches one by one */
static int matchregex(void*__restrict__ x,const char*__restrict__ s,int ofs,struct __regex_t*__restrict__ preg,int plus,int eflags) {
  register struct regex* a=(struct regex*)x;
  int i,tmp;
  void *save;
#ifdef DEBUG
  printf("%p matching regex against \"%.20s\"\n",a,s);
#endif
  a->startofs = ofs;
  for (i=0; i<a->num; ++i) {
    dprintf("matchregex (%p) trying branch %d of %d\n",x,i,a->num);
    assert(a->b[i].m==matchbranch || a->b[i].m==matchempty);
    tmp=a->b[i].m(&a->b[i],s,ofs,preg,plus,eflags);
    if (tmp>=0) {
      dprintf("matchregex (%p) branch %d matched %d characters\n",x,i,tmp);
      return tmp;
    }
  }
  dprintf("matchregex (%p) return -1\n",x);
  return -1;
}

static const char* parseregex(struct regex*__restrict__ r,const char*__restrict__ s,regex_t*__restrict__ p) {
  struct branch b;
  const char *tmp;
#ifdef DEBUG
  printf("parseregex: %c\n", *s);
#endif
  r->m=matchregex;
  r->num=0; r->b=0; r->pieces=0;
  b.next=0;
  if (*s==')' || !*s) {
    r->m=matchempty;
    return s;
  }
  for (;;) {
    tmp=parsebranch(&b,s,p,&r->pieces);
    if (tmp==s && *tmp!='|' && !closebracket(s,p)) return s;
    //printf("r->b from %p to ",r->b);
    {
      struct branch* t;
      if (!(t=realloc(r->b,++r->num*sizeof(b)))) {
	free(b.p);
	return s;
      }
      //printf("branch realloc: %p -> %p (%d*%d)\n",r->b,t,r->num,(int)sizeof(b));
      r->b=t;
    }
    //printf("%p (size %d)\n",r->b,(int)(r->num*sizeof(b)));
    r->b[r->num-1]=b;
    if (closebracket(s,p)) {
      r->b[r->num-1].m=matchempty;
      return s;
    }
    dprintf("parseregex: assigned branch %d at %p\n",r->num-1,r);
    s=tmp;
    if (closebracket(s,p)) return s;
    if (*s=='|') ++s;
  }
  return tmp;
}


/* The matcher relies on the presence of next pointers, of which the
 * parser does not know the correct destination.  So we need an
 * additional pass through the data structure that sets the next
 * pointers correctly. */
static void regex_putnext(struct regex* r,void* next);

static void atom_putnext(struct atom*__restrict__ a,void*__restrict__ next) {
  a->next=next;
  if (a->type==REGEX)
    regex_putnext(&a->u.r,next);
}

static void piece_putnext(struct piece*__restrict__ p,void*__restrict__ next) {
  p->next=next;
  atom_putnext(&p->a,next);
}

static void branch_putnext(struct branch*__restrict__ b,void*__restrict__ next) {
  int i;
  if (b->m!=matchempty) {
    for (i=0; i<b->num-1; ++i) {
      if (b->p[i+1].min==1 && b->p[i+1].max==1)
/* shortcut: link directly to next atom if it's a piece with min=max=1 */
	piece_putnext(&b->p[i],&b->p[i+1].a);
      else
	piece_putnext(&b->p[i],&b->p[i+1]);
    }
    piece_putnext(&b->p[i],next);
  }
  b->next=next;
}

static void regex_putnext(struct regex*__restrict__ r,void*__restrict__ next) {
  int i;
  for (i=0; i<r->num; ++i)
    branch_putnext(&r->b[i],next);
  r->next=next;
}



int regcomp(regex_t*__restrict__ preg, const char*__restrict__ regex, int cflags) {
  const char* t;
  preg->cflags=cflags;
  preg->brackets=0;
  t=parseregex(&preg->r,regex,preg);
  if (t==regex && *regex!=0) return -1;
  regex_putnext(&preg->r,0);
  return 0;
}

int regexec(const regex_t*__restrict__ preg, const char*__restrict__ string, size_t nmatch, regmatch_t pmatch[], int eflags) {
  int matched;
  const char *orig=string;
  assert(preg->brackets+1>0 && preg->brackets<1000);
  for (matched=0; (unsigned int)matched<nmatch; ++matched)
    pmatch[matched].rm_so=-1;
#ifdef DEBUG
  printf("alloca(%d)\n",(int)sizeof(regmatch_t)*(preg->brackets+3));
#endif
  ((regex_t*)preg)->l=alloca(sizeof(regmatch_t)*(preg->brackets+3));
  for(matched=0;matched<preg->brackets+3;++matched)
    preg->l[matched].rm_so=-1;
  int earlyabort=(preg->cflags&REG_NEWLINE)==0;
  if (earlyabort) {
    int i;
    for (i=0; i<preg->r.num; ++i)
      if (preg->r.b[i].p->a.type!=LINESTART) {
	earlyabort=0;
	break;
      }
  }
  while (1) {
    matched=preg->r.m((void*)&preg->r,string,string-orig,(regex_t*)preg,0,eflags);
//    printf("ebp on stack = %x\n",stack[1]);
    if (__unlikely(matched>=0)) {
      preg->l[0].rm_so=string-orig;
      preg->l[0].rm_eo=string-orig+matched;
      dprintf("regexec: bnum: %d -> rm_so=%d, rm_eo=%d\n",0,(int)(string-orig),(int)(string-orig+matched));
      if ((preg->cflags&REG_NOSUB)==0) {
        if (preg->brackets+1 < (int)nmatch) nmatch = (size_t)preg->brackets+1;
        memcpy(pmatch,preg->l,nmatch*sizeof(regmatch_t));
      }
      return 0;
    }
    if (!*string) break;
    ++string;
    eflags|=REG_NOTBOL;
    /* we are no longer at the beginning of the line, so if our regex
     * starts with ^, we can skip trying to run it on the rest of the
     * line */
    if (earlyabort) break;
  }
  return REG_NOMATCH;
}

static void __regfree(struct regex* r) {
  int i;
  for (i=0; i<r->num; ++i) {
    int j,k;
    k=r->b[i].num;
    for (j=0; j<k; ++j)
      if (r->b[i].p[j].a.type==REGEX)
	__regfree(&r->b[i].p[j].a.u.r);
      else if (r->b[i].p[j].a.type==STRING)
	free(r->b[i].p[j].a.u.s.s);
    free(r->b[i].p);
  }
  free(r->b);
}

void regfree(regex_t* preg) {
  __regfree(&preg->r);
  memset(preg,0,sizeof(regex_t));
}

size_t regerror(int errcode, const regex_t*__restrict__ preg, char*__restrict__ errbuf, size_t errbuf_size) {
  (void)preg;
  (void)errcode;
  strncpy(errbuf,"invalid regular expression (sorry)",errbuf_size);
  return strlen(errbuf);
}




#if 0
int main() {
  struct regex r;
  int bnum=-1;
  const char* t=parseregex(&r,"^a*ab$",&bnum);
  regex_putnext(&r,0);
  printf("%d pieces, %s\n",r.pieces,t);
  printf("%d\n",r.m(&r,"aaab",0,0,0));
  return 0;
}
#endif
