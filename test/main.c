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
#include <string.h>
#include "../src/server/server.h"

void home_handler(response_t* resp, request_t* req) {
  add_header_field(&resp->header, "Content-Type", "text/html");
  send_http_response(resp, "<h1>Boas-vindas à página Home</h1>");
}

int main() {
  
  server_t server = {0};
  new_http_server(&server, "0.0.0.0", "http");

  handle_route(&server, "/home/", home_handler, GET);
  
  start_listening(&server);
  return 0;
}