#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

extern int __ltostr(char *s, int size, unsigned long i, int base, char UpCase);

int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
		size_t hostlen, char *serv, size_t servlen, int flags) {
  sa_family_t f=((struct sockaddr_storage *)sa)->ss_family;
  if (host && hostlen>0) {	/* user wants me to resolve the host name */
    register const char*addr=(f==AF_INET6)?(char*)&((struct sockaddr_in6*)sa)->sin6_addr:(char*)&((struct sockaddr_in*)sa)->sin_addr;
    if (flags&NI_NUMERICHOST) {
      if (!inet_ntop(f,addr,host,hostlen))
	return -1;
    } else {
      char buf[4096];
      struct hostent h;
      struct hostent *H;
      int herrno;
      if (gethostbyaddr_r(addr,f==AF_INET6?16:4,f,&h,buf,4096,&H,&herrno))
	return -1;
      strncpy(host,h.h_name,hostlen-1);
      host[hostlen-1]=0;
    }
  }
  if (serv && servlen>0) {
    register short int port=(f==AF_INET6)?((struct sockaddr_in6*)sa)->sin6_port:((struct sockaddr_in*)sa)->sin_port;
    if (flags&NI_NUMERICSERV) {
      __ltostr(serv,servlen,port,10,0);
    } else {
      struct servent *s;
      if (!(s=getservbyport(port,flags&NI_DGRAM?"udp":"tcp")))
	return -1;
    }
  }
  return 0;
}
