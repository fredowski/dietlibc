#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include "dietfeatures.h"
#include "dietdns.h"

extern void __dns_make_fd(void);
extern int __dns_fd;

extern void __dns_readstartfiles(void);

extern int __dns_decodename(unsigned char *packet,unsigned int offset,unsigned char *dest,unsigned int maxlen);

/* Oh boy, this interface sucks so badly, there are no words for it.
 * Not one, not two, but _three_ error signalling methods!  (*h_errnop
 * nonzero?  return value nonzero?  *RESULT zero?)  The glibc goons
 * really outdid themselves with this one. */
#ifdef WANT_FULL_RESOLV_CONF
static int __dns_gethostbyx_r_inner(const char* name, struct hostent* result,
			char *buf, size_t buflen,
			struct hostent **RESULT, int *h_errnop, int lookfor);

static int __dns_gethostbyx_r_inner(const char* name, struct hostent* result,
			char *buf, size_t buflen,
			struct hostent **RESULT, int *h_errnop, int lookfor) {
#else
int __dns_gethostbyx_r(const char* name, struct hostent* result,
			char *buf, size_t buflen,
			struct hostent **RESULT, int *h_errnop, int lookfor) {
#endif
  int names,ips;
  unsigned char *cur;
  unsigned char *max;
  unsigned char packet[512];
  int size;
  __dns_make_fd();

  if (lookfor==1) {
    result->h_aliases=(char**)(buf+8*4);
    result->h_addrtype=AF_INET;
    result->h_length=4;
    result->h_addr_list=(char**)buf;
  } else {
    result->h_aliases=(char**)(buf+8*16);
    result->h_addrtype=AF_INET6;
    result->h_length=16;
    result->h_addr_list=(char**)buf;
  }
  result->h_aliases[0]=0;

  cur=buf+16*sizeof(char*);
  max=buf+buflen;
  names=ips=0;

  if ((size=res_mkquery(QUERY,name,C_IN,lookfor,0,0,0,packet,512))<0) return 1;
  {
    {
      int i;	/* current server */
      int j;	/* timeout count down */
      struct pollfd duh;
      struct timeval last,now;
      i=0;
      __dns_readstartfiles();
      duh.fd=__dns_fd;
      duh.events=POLLIN;
      last.tv_usec=0;
      for (j=10; j>0; --j) {
	gettimeofday(&now,0);
	if (now.tv_sec-last.tv_sec>10) {
	  sendto(__dns_fd,packet,(size_t)size,0,(struct sockaddr*)&(_res.nsaddr_list[i]),sizeof(struct sockaddr));
	  gettimeofday(&last,0);
	}
	if (++i > _res.nscount) i=0;
	if (poll(&duh,1,15000) == 1) {
	  /* read and parse answer */
	  unsigned char inpkg[1500];
	  char *tmp;
	  /*int len=*/ read(__dns_fd,inpkg,1500);
#if 0
	  {
	    int tft=open("duh",0);
	    read(tft,inpkg,1500);
	    close(tft);
	  }
#endif
	  /* header, question, answer, authority, additional */
	  if (inpkg[0]!=packet[0] || inpkg[1]!=packet[1]) continue;	/* wrong ID */
	  if ((inpkg[2]&0xf9) != (_res.options&RES_RECURSE?0x81:0x80)) continue;	/* not answer */
	  if ((inpkg[3]&0x0f) != 0) {
	    *h_errnop=HOST_NOT_FOUND;
	    return 1;
	  }
	  tmp=inpkg+12;
	  {
	    char Name[257];
	    unsigned short q=((unsigned short)inpkg[4]<<8)+inpkg[5];
	    while (q>0) {
	      while (*tmp) tmp+=*tmp+1;
	      tmp+=5;
	      --q;
	    }
	    q=((unsigned short)inpkg[6]<<8)+inpkg[7];
	    if (q<1) goto nodata;
	    while (q>0) {
	      int decofs=__dns_decodename(inpkg,(size_t)(tmp-(char*)inpkg),Name,256);
	      if (decofs<0) break;
	      tmp=inpkg+decofs;
	      --q;
	      if (tmp[0]!=0 || tmp[1]!=lookfor ||	/* TYPE != A */
	          tmp[2]!=0 || tmp[3]!=1) {		/* CLASS != IN */
		if (tmp[1]==5) {	/* CNAME */
		  tmp+=10;
		  decofs=__dns_decodename(inpkg,(size_t)(tmp-(char*)inpkg),Name,256);
		  if (decofs<0) break;
		  tmp=inpkg+decofs;
		} else
		  break;
		continue;
	      }
	      tmp+=10;	/* skip type, class, TTL and length */
	      {
		int slen;
		if (lookfor==1 || lookfor==28) /* A or AAAA*/ {
		  slen=strlen(Name);
		  if (cur+slen+8+(lookfor==28?12:0)>=max) { *h_errnop=NO_RECOVERY; return 1; }
		} else if (lookfor==12) /* PTR */ {
		  decofs=__dns_decodename(inpkg,(size_t)(tmp-(char*)inpkg),Name,256);
		  if (decofs<0) break;
		  tmp=inpkg+decofs;
		  slen=strlen(Name);
		} else
		  slen=strlen(Name);
		strcpy(cur,Name);
		if (names==0)
		  result->h_name=cur;
		else
		  result->h_aliases[names-1]=cur;
		result->h_aliases[names]=0;
		++names;
/*		cur+=slen+1; */
		cur+=(slen|3)+1;
		result->h_addr_list[ips++] = cur;
		if (lookfor==1) /* A */ {
		  *(int*)cur=*(int*)tmp;
		  cur+=4;
		  result->h_addr_list[ips]=0;
		} else if (lookfor==28) /* AAAA */ {
		  {
		    int k;
		    for (k=0; k<16; ++k) cur[k]=tmp[k];
		  }
		  cur+=16;
		  result->h_addr_list[ips]=0;
		}
	      }
/*	      puts(Name); */
	    }
	  }
	  if (!names) {
nodata:     *h_errnop=NO_DATA;
	    return 1;
	  }
/*	  printf("%d answers\n",((unsigned short)inpkg[6]<<8)+inpkg[7]);
	  printf("ok\n");*/
	  *h_errnop=0;
	  *RESULT=result;
	  return 0;
	} /* timeout */
/*kaputt:*/
      }
    }
  }
  *h_errnop=TRY_AGAIN;
  return 1;
}

#ifdef WANT_FULL_RESOLV_CONF
extern int __dns_search;
extern char *__dns_domains[];

int __dns_gethostbyx_r(const char* name, struct hostent* result,
			char *buf, size_t buflen,
			struct hostent **RESULT, int *h_errnop, int lookfor) {
  const char *tmp=name;
  char Buf[MAXDNAME+1];
  int res;
  size_t len=strlen(name);
  int count=0;
  memmove(Buf,name,len);
  Buf[len]=Buf[MAXDNAME]=0;
//  printf("appending %d: %p\n",count,__dns_domains[count]);
  while ((res=__dns_gethostbyx_r_inner(tmp,result,buf,buflen,RESULT,h_errnop,lookfor))) {
    if (count==__dns_search) break;
    Buf[len]='.';
//    printf("appending %d: %p (%s)\n",count,__dns_domains[count],__dns_domains[count]);
    strncpy(Buf+len+1,__dns_domains[count],MAXDNAME-len-1);
    tmp=Buf;
    ++count;
  }
  return res;
}
#endif

