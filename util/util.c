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
#include <string.h>
#include <sys/socket.h>
#include "util.h"

/*
  Exibe a mensagem de erro correspondente ao retorno de getaddrinfo e
  encerra o programa

  @param func_name: nome da função que deu erro
  @param err: retorno de getaddrinfo
*/
void net_panic(const char* func_name, const int err) {
  fprintf(stderr, "%s: %s\n", func_name, gai_strerror(err));
  exit(EXIT_FAILURE);
}

/*
  Exibe a mensagem de erro correspondente à errno e encerra o programa

  @param func_name: nome da função que deu erro
*/
void errno_panic(const char* func_name) {
  perror(func_name);
  exit(EXIT_FAILURE);
}

/*
  Retorna a quantidade de vezes que um caractere aparece em uma string

  @param buffer: string que será analisada
  @param c: caractere a ser contado
*/
int char_counter(char* buffer, char c) {
  int counter = 0;
  for(; *buffer; buffer++)
    if(*buffer == c)
      counter++;
  return counter;
}

char* make_copy(char* buffer) {
  size_t len = strlen(buffer) + 1;
  
  char* copy = calloc(len, sizeof(char));
  if(copy == NULL)
    errno_panic("make_copy: calloc");

  memcpy(copy, buffer, len);
  return copy;
}