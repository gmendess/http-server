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
  add_header_field(&resp->header, "Content-Type", "application/xml");

  int err = send_http_response(resp, "<person><name>Gabriel</name><age>20</age></person>");
  if(err != 0)
    http_error("send_http_response", err);
}

int main() {
  int err = 0;
  server_t server = {0};
  
  if( (err = new_http_server(&server, "0.0.0.0", "http")) != 0) {
    http_error("new_http_server", err);
    return EXIT_FAILURE;
  }

  if( (err = handle_route(&server, "/api/getPerson", home_handler, GET)) != 0)
    http_error("handle_route", err);

  if( (err = start_listening(&server)) != 0) {
    http_error("start_listening", err);
    return EXIT_FAILURE;
  }

  return 0;
}