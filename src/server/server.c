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
#include "../errors/errors.h"
#include "../http/request/request.h"
#include "../http/response/response.h"

/*
  Percorre a lista encadeada @ai tentando criar um socket e associá-lo à algum endereço dessa lista

  @param server: servidor que terá o socket associado à um endereço de @ai
  @param ai: lista encadeada de endereços(retornada por getaddrinfo) em que um de seus endereços
   será associado ao servidor 
*/
static int bind_server(server_t* server, struct addrinfo* ai) {
  int listener = 0; // file descriptor do socket do servidor

  // ponteiro auxiliar para percorrer a lista
  struct addrinfo* aux = NULL;

  // percorre lista encadeada de endereços até conseguir criar socket e dar bind
  for(aux = ai; aux != NULL; aux = aux->ai_next) {

    // tenta adquirir um socket com as configurações do endereço em 'aux'
    listener = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
    if(listener == -1) {
      perror("bind_server: socket");
      continue;
    }

    // tenta reaproveitar o endereço e serviço caso já esteja em uso 
    int boolean = 1; // true
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &boolean, sizeof(boolean));

    // tenta dar bind no endereço contido em 'aux'
    if(bind(listener, aux->ai_addr, aux->ai_addrlen) == -1) {
      perror("bind_server: bind");
      continue;
    }

    // se chegar até aqui, foi possível adquirir um socket e dar bind, encerra o loop
    break;
  }

  // se aux == NULL, cheguei ao final da lista sem conseguir criar o socket e dar bind
  if(aux == NULL)
    return ERR_BIND;

  get_addr_and_port(aux->ai_addr, &server->port, server->addr, sizeof(server->addr));
  server->listener = listener;

  // não preciso mais de 'ai', libero sua memória
  freeaddrinfo(ai);

  return 0;
}

/*
  Libera os recursos alocados para um server_t, no caso, apenas a lista 
  encadeada de rotas.

  @param server: servidor que terá os recursos desalocados
*/
static void free_server(server_t* server) {
  route_t* aux = server->route;
  route_t* prev = NULL;
  while(aux) {
    prev = aux;
    aux = aux->next;
    free(prev);
  }

  if(server->listener >= 0)
    close(server->listener);
}

/*
  Função responsável por lidar com a requisição de um cliente

  @param server: informações do servidor
  @param clientfd: file descriptor para se comunicar com o cliente
*/
static void handle_request(server_t* server, int clientfd) {
  int err = 0; // controle de erros

  char buffer[1024] = {0};
  recv(clientfd, buffer, sizeof(buffer), 0);

  request_t req = {0};
  if( (err = parse_request(buffer, &req)) != 0 ) {
    http_error("parse_request", err);
    exit(EXIT_FAILURE);
  }

  // response padrão
  response_t resp = {0};
  resp.clientfd = clientfd;
  resp.status = OK;
  add_header_field(&resp.header, "Date", gmt_date_now());

  route_t* route = find_route(server, req.req_line.path, req.req_line.method);
  if(route == NULL)
    send_default_404(&resp);
  else if(route == (route_t*) -1)
    send_default_405(&resp);
  else
    route->handler(&resp, &req);

  free_request(&req);
  free_header(&resp.header);
  free_server(server);

  exit(EXIT_SUCCESS);
}

/*
  Cria um novo servidor http

  @param server: estrutura que conterá informações do servidor
  @param addr: endereço do servidor 
  @param port: serviço (porta)
*/
int new_http_server(server_t* server, const char* addr, const char* port) {
  int err = 0; // controle de erros

  // lista de rotas vazia
  server->route = NULL;

  struct addrinfo hints = {0};
  hints.ai_family   = AF_UNSPEC;   // IPv4 ou IPv6
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags    = AI_PASSIVE;

  // retorno de getaddrinfo
  struct addrinfo* ai = NULL;

  // adquire lista de endereços do host/serviço
  err = getaddrinfo(addr, port, &hints, &ai);
  if(err != 0)
    return ERR_ADDRLIST;

  // associado o servidor com algum endereço disponível na lista 'ai'
  if( (err = bind_server(server, ai)) != 0)
    return err;

  return 0;
}

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.
  Retorna diferente de 0 em caso de erro

  @param server: estrutura server_t contendo informações do servidor a ser iniciado 
*/
int start_listening(server_t* server) {
  // coloca o socket em estado de listening, escutando por requição de conexão
  if(listen(server->listener, BACKLOG) == -1)
    return ERR_LISTEN;

  // configurando handler para SIGCHLD
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
    int clientfd = accept(server->listener, (struct sockaddr*) &ss, &size);
    if(clientfd == -1) {
      perror("accept");
      continue;
    }

    // cria um processo filho para lidar com a requisição
    pid_t pid = fork();
    if(pid == 0) {
      // processo filho
      close(server->listener); // fecha o socket do listener, pois o processo filho não precisa dele
      server->listener = -1;   // configura o socket como um inválido, por favor, não usar
      handle_request(server, clientfd); // lida com a requisição
    }
    else if(pid > 0) {
      // processo pai
      close(clientfd); // fecha o socket do cliente, pois o processo pai não precisa dele
      puts("Requisição aceita!");
    }
    else
      perror("fork");

  }

  free_server(server);
  return 0;
}

/*
  Adiciona um handler para um determinada rota do servidor
  Retorna diferente de 0 em caso de erro

  @param server: servidor que passará a lidar com a rota especificada em @route
  @param route: nome da rota
  @param handler: função handler que será executada quando um cliente acessar @route 
  @param m_code: código do método HTTP (veja methods.h)
*/
int handle_route(server_t* server,
                 const char* route,
                 route_handler_t handler,
                 http_method_t   method)
{
  if(method < 0 || method > 6)
    return ERR_HTTPMET;
  else if(!route || route[0] == '\0')
    return ERR_ROUTE;
  else if(!server)
    return ERR_SERVER;

  // aloca memória para um route_t
  route_t* new_route = must_calloc(1, sizeof(route_t));

  // configurando route_t
  new_route->path    = route;
  new_route->handler = handler;
  new_route->method  = method;
  new_route->next    = server->route;
  // ligando a nova rota à lista de rotas
  server->route      = new_route;

  return 0;
}

/*
  Procura uma rota de nome @path contida na lista de rotas do servidor
  Retorna ponteiro para a rota ou NULL, caso não encontrada

  @param server: servidor que será executada a busca pela rota
  @param path: nome da rota
*/
route_t* find_route(server_t* server, const char* path, http_method_t method) {
  route_t* aux = server->route;
  route_t* route_found = NULL;
  while(aux != NULL) {
    if(strcmp(aux->path, path) == 0) {
      route_found = (route_t*) -1;
      if(aux->method == method)
        return aux; // rota encontrada
    }
    aux = aux->next;
  }

  // se a rota foi encontrada mas o método HTTP não corresponde, retorna (route_t*) -1
  // XXX: tenho que melhorar isso ai, pq é meio gambiarra
  if(route_found != NULL)
    return route_found;

  return NULL;
}