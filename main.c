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

  struct sockaddr_storage ss;
  int size = sizeof(ss);

  int client = accept(listener, (struct sockaddr*) &ss, &size);
  if(client == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  char buffer[1024] = {0};
  recv(client, buffer, sizeof(buffer), 0);
  puts(buffer);

  // enviando mensagem para o cliente
  send(client, "<h1>Teste</h1>", 14, 0);

  return 0;
}