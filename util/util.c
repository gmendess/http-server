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

/*
  Exibe a mensagem de erro correspondente ao retorno de getaddrinfo e
  encerra o programa
*/
void net_panic(const char* func_name, const int err) {
  fprintf(stderr, "%s: %s\n", func_name, gai_strerror(err));
  exit(EXIT_FAILURE);
}

/*
  Exibe a mensagem de erro correspondente à errno e encerra o programa
*/
void errno_panic(const char* func_name) {
  perror(func_name);
  exit(EXIT_FAILURE);
}