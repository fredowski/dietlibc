#include <netdb.h>

const char* hstrerror(int h_errno) {
  switch (h_errno) {
  case 0: return "OK";
  case TRY_AGAIN: return "Temporary failure.";
  case HOST_NOT_FOUND:
  default: return "Unknown host";
  }
}
