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
#include "server.h"
#include "../util/util.h"
#include "../http/request/request.h"

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
    
    // se chegar até aqui, foi possível adquirir um socket e dar bind, encerra o loop
    break;
  }

  // se aux == NULL, cheguei ao final da lista sem conseguir criar o socket e dar bind
  if(aux == NULL)
    return -1;
  
  // sucesso
  return listener;
}

/*
  Função responsável por lidar com a requisição de um cliente

  @param clientfd: file descriptor para se comunicar com o cliente
*/
static void handle_request(int clientfd) {
  // processo filho
  char buffer[1024] = {0};
  recv(clientfd, buffer, sizeof(buffer), 0);

  int counter = char_counter(buffer, '\n');
  char** lines = NULL;
  counter = parse_lines(buffer, &lines, counter);

  request_t req = {0};
  parse_request_line(lines[0], &req);
  printf("Method: \"%s\"\n", req.method.name);
  printf("Method code: \"%d\"\n", req.method.code);
  printf("Path: \"%s\"\n", req.path);
  printf("Version: \"%s\"\n", req.version);

  // char* resp = {
  //   "HTTP/1.1 200 OK\n"\
  //   "Content-Type: text/xml\n"\
  //   "Content-Length: 50\n\n"\
  //   "<person><name>Gabriel</name><age>19</age></person>"\
  // };

  // if(send(clientfd, resp, strlen(resp), 0) == -1)
  //   perror("send");

  exit(EXIT_SUCCESS);
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

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.

  @param listener: file descriptor do servidor; retornado por new_http_server 
*/
void start_listening(int listener) {
  // coloca o socket em estado de listening, escutando por requição de conexão
  if(listen(listener, BACKLOG) == -1)
    errno_panic("start_listening: listen");

  // loop infinito para ficar aceitando por requisições
  while(1) {
    struct sockaddr_storage ss;
    unsigned size = sizeof(ss);

    int clientfd = accept(listener, (struct sockaddr*) &ss, &size);
    if(clientfd == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if(pid == 0) {
      // processo filho
      close(listener);
      handle_request(clientfd);
    }
    else if(pid > 0) {
      // processo pai
      close(clientfd);
      puts("Requisição aceita!");
    }
    else
      perror("fork");

  }
}