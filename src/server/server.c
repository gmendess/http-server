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

#ifndef __USE_XOPEN2K8
#define __USE_XOPEN2K8
#endif

#include <netdb.h>
#include <signal.h>
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
  char buffer[1024] = {0};
  recv(clientfd, buffer, sizeof(buffer), 0);
  char* copy = make_copy(buffer);

  request_t req = {0};
  parse_request(buffer, &req);

  puts("REQUEST LINE");
  printf("\tMethod: %s\n", req.req_line.method.name);
  printf("\tPath: %s\n", req.req_line.path);
  printf("\tVersion: %s\n", req.req_line.version);

  puts("\nHEADER");
  for(int x = 0; x < req.header_counter; x++)
    printf("\t%s\n", req.header[x]);
  
  puts("\nBODY");
  puts(req.body);

  puts("\n----------------------------\n");
  puts(copy);

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

  // configurando handler para SIGSEGV para que exiba uma mensagem de erro e encerre o programa,
  // caso o sinal venha a ser recebido
  struct sigaction sa = {0};
  sa.sa_handler = sigchld_handler; // função handler do sinal
  sa.sa_flags   = SA_RESTART;      // reinicia uma syscall que foi interrompida pelo sinal
  sigemptyset(&sa.sa_mask);        // zera a máscara de sinais a serem bloqueados
  sigaction(SIGCHLD, &sa, NULL);   // configura a ação a ser tomada ao receber um SIGCHLD

  // loop infinito para ficar aceitando por requisições
  while(1) {
    struct sockaddr_storage ss;
    unsigned size = sizeof(ss);

    // aceita uma conexão com o cliente
    int clientfd = accept(listener, (struct sockaddr*) &ss, &size);
    if(clientfd == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    // cria um processo filho para lidar com a requisição
    pid_t pid = fork();
    if(pid == 0) {
      // processo filho
      close(listener); // fecha o socket do listener, pois o processo filho não precisa dele
      handle_request(clientfd); // lida com a requisição
    }
    else if(pid > 0) {
      // processo pai
      close(clientfd); // fecha o socket do cliente, pois o processo pai não precisa dele
      puts("Requisição aceita!");
    }
    else
      perror("fork");

  }
}