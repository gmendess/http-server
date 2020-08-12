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
#include "server.h"
#include "../util/util.h"

static int create_socket_and_bind(struct addrinfo* ai) {
  int listener = 0; // file descriptor do socket do servidor; retornado pela função
  
  // ponteiro auxiliar para percorrer a lista
  struct addrinfo* aux = NULL;

  // percorre lista encadeada de endereços até conseguir criar socket e dar bind
  for(aux = ai; aux != NULL; aux = aux->ai_next) {
    
    // tenta adquirir um socket com as configurações do endereço em 'aux'
    listener = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
    if(listener == -1) {
      perror("new_http_server: socket");
      continue;
    }

    // tenta reaproveitar o endereço e serviço caso já esteja em uso 
    int boolean = 1; // true
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &boolean, sizeof(boolean));

    // tenta dar bind no endereço contido em 'aux'
    if(bind(listener, aux->ai_addr, aux->ai_addrlen) == -1) {
      perror("new_http_server: bind");
      continue;
    }

    // coloca o socket em estado de listening, escutando por requição de conexão
    if(listen(listener, BACKLOG) == -1) {
      perror("new_http_server: listen");
      continue;
    }
    
    // se chegar até aqui, foi possível adquirir um socket e dar bind, encerra o loop
    break;
  }

  // se aux == NULL, cheguei ao final da lista sem conseguir criar o socket e dar bind
  if(aux == NULL)
    return -1;
  
  // sucesso
  return listener;
}

int new_http_server(const char* addr, const char* port) {

  // file descriptor do socket do servidor; retornado pela função
  int listener = 0;

  struct addrinfo hints = {0};
  hints.ai_family   = AF_UNSPEC;   // IPv4 ou IPv6
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags    = AI_PASSIVE;

  // retorno de getaddrinfo
  struct addrinfo* ai = NULL;

  // adquire lista de endereços do host/serviço
  int err = getaddrinfo(addr, port, &hints, &ai);
  if(err != 0)
    net_panic("new_http_server: getaddrinfo", err);

  // adquire um socket já configurado para escutar por conexões
  listener = create_socket_and_bind(ai);
  if(listener == -1)
    errno_panic("new_http_server: create_socket_and_bind");

  return listener;
}