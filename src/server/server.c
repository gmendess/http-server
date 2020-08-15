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

static void bind_server(server_t* server, struct addrinfo* ai) {
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

  // não preciso mais de 'ai', libero sua memória
  freeaddrinfo(ai);

  // se aux == NULL, cheguei ao final da lista sem conseguir criar o socket e dar bind
  if(aux == NULL)
    panic("bind_server", "não foi possível atribuir nenhum endereço ao servidor");
  
  get_addr_and_port(aux->ai_addr, &server->port, server->addr, sizeof(server->addr));
  server->listener = listener;
}

/*
  Função responsável por lidar com a requisição de um cliente

  @param server: informações do servidor
  @param clientfd: file descriptor para se comunicar com o cliente
*/
static void handle_request(server_t* server, int clientfd) {
  char buffer[1024] = {0};
  recv(clientfd, buffer, sizeof(buffer), 0);

  request_t req = {0};
  parse_request(buffer, &req);
  puts(req.req_line.path);
  if(strcmp(req.req_line.path, server->route->path) == 0) {
    if(req.req_line.method.code == server->route->m_code)
      server->route->handler(clientfd, &req);
    else
      write(clientfd, "Metodo nao disponivel para essa rota!\n", 38);
  }
  else {
    write(clientfd, "Pagina nao encontrada!\n", 23);
  }

  free_request(&req);
  exit(EXIT_SUCCESS);
}

/*
  Cria um novo servidor http

  @param server: estrutura que conterá informações do servidor
  @param addr: endereço do servidor 
  @param port: serviço (porta)
*/
void new_http_server(server_t* server, const char* addr, const char* port) {

  // lista de rotas vazia
  server->route = NULL;

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

  // associado o servidor com algum endereço disponível na lista 'ai'
  bind_server(server, ai);
}

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.

  @param server: estrutura server_t contendo informações do servidor a ser iniciado 
*/
void start_listening(server_t* server) {
  // coloca o socket em estado de listening, escutando por requição de conexão
  if(listen(server->listener, BACKLOG) == -1)
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
    int clientfd = accept(server->listener, (struct sockaddr*) &ss, &size);
    if(clientfd == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    // cria um processo filho para lidar com a requisição
    pid_t pid = fork();
    if(pid == 0) {
      // processo filho
      close(server->listener); // fecha o socket do listener, pois o processo filho não precisa dele
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
}

/*
  Adiciona um handler para um determinada rota do servidor

  @param server: servidor que passará a lidar com a rota especificada em @route
  @param route: nome da rota
  @param handler: função handler que será executada quando um cliente acessar @route 
  @param m_code: código do método HTTP (veja methods.h)
*/
void handle_route(server_t* server,
                  const char* route,
                  route_handler_t handler,
                  http_method_code_t m_code) 
{
  // aloca memória para um route_t
  route_t* new_route = calloc(1, sizeof(route_t));
  if(!new_route)
    errno_panic("handle_route: calloc");

  // configurando route_t
  new_route->path    = route;
  new_route->handler = handler;
  new_route->m_code  = m_code;
  
  // percorre a lista de rotas do server até chegar no final
  route_t* aux  = server->route;
  route_t* prev = NULL;
  while(aux) {
    prev = aux;
    aux = aux->next;
  }

  // se prev for NULL, 'server' não possui nenhuma rota ainda
  if(prev == NULL)
    server->route = new_route;
  else
    prev->next = new_route;
}