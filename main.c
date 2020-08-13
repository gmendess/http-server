#include <stdlib.h>
#include <stdio.h>

#ifndef __USE_POSIX
#define __USE_POSIX
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "server/server.h"

int main() {
  
  int listener = new_http_server("0.0.0.0", "http");
  start_listening(listener);

  return 0;
}