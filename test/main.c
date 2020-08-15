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

void home_handler(int clientfd, request_t* req) {
  char* resp = {
    "HTTP/1.1 200 OK\n"\
    "Content-Type: text/html\n"\
    "Content-Length: 35\n\n"\
    "<h1>Oi, essa eh a pagina /home/<h1>"\
  };
  write(clientfd, resp, strlen(resp));
}

void home_post_handler(int clientfd, request_t* req) {
  char* resp = {
    "HTTP/1.1 200 OK\n"\
    "Content-Type: text/html\n"\
    "Content-Length: 44\n\n"\
    "<h1>Oi, essa eh a de post poggers /home/<h1>"\
  };
  write(clientfd, resp, strlen(resp));
}

int main() {
  
  server_t server = {0};
  new_http_server(&server, "0.0.0.0", "http");

  handle_route(&server, "/home/", home_handler, GET);
  handle_route(&server, "/home/", home_post_handler, POST);
  
  start_listening(&server);

  return 0;
}