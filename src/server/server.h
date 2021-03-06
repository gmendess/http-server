#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

#include "../http/request/request.h"
#include "../http/response/response.h"
#include "../http/request/methods/methods.h"
#include "../errors/errors.h"
#include "thread-pool/tpool.h"

/*
  Fila de espera do socket; usado na syscall 'listen'
*/
#define BACKLOG 5

// função handler para um rota 
typedef void(*route_handler_t)(response_t*, request_t*);

typedef struct __route_t {
  const char* path;          // caminho especificado na requisição
  http_method_t     method;  // método http
  route_handler_t   handler; // handler da rota
  struct __route_t* next;    // próxima rota (NULL = fim da lista)
} route_t;

typedef enum {
  FORKED,  // cada conexão é igual a um processo
  #define FORKED   FORKED
  THREADED // cada conexão é igual a uma thread
  #define THREADED THREADED
} server_type_t;

typedef struct {
  server_type_t type;  // threaded server ou forked server (a base de threads ou processos)
  int listener;        // file descriptor do servidor (não usar no processo filho)
  char addr[INET6_ADDRSTRLEN]; // endereço do servidor
  int  port;           // porta do servidor
  route_t* route;      // lista encadeada das rotas do servidor
  conn_queue_t* queue; // fila de conexões (apenas p/ threaded-server, caso contrário será NULL)
} server_t;

/*
  Cria um novo servidor http
  Retorna diferente de 0 em caso de erro

  @param server: estrutura que conterá informações do servidor
  @param addr: endereço do servidor 
  @para port: serviço (porta)
*/
int new_http_server(server_t* server, const char* addr, const char* port);

/*
  Configura o servidor para ser um threaded server, ou seja, ele usará uma thread para lidar com cada
  conexão. Essas threads fazem parte de uma thread pool.

  @param server: servidor a ser configurado
  @param queue_size: tamanho da fila de conexões; cada conexão aceita será enfileirada e posteriormente
   pega por uma thread que estiver disponível na pool. 
*/
void config_threaded_server(server_t* server, size_t queue_size);

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.
  Retorna diferente de 0 em caso de erro

  @param server: estrutura server_t contendo informações do servidor a ser iniciado 
*/
int start_listening(server_t* server);

/*
  Adiciona um handler para um determinada rota do servidor
  Retorna diferente de 0 em caso de erro.

  @param server: servidor que passará a lidar com a rota especificada em @route
  @param route: nome da rota
  @param handler: função handler que será executada quando um cliente acessar @route 
  @param method: método HTTP
*/
int handle_route(server_t* server,
                 const char* route,
                 route_handler_t handler,
                 http_method_t   method);

/*
  Procura uma rota de nome @path contida na lista de rotas do servidor
  Retorna diferente de 0 em caso de erro

  @param server: servidor que será executada a busca pela rota
  @param path: nome da rota
  @param method: método HTTP
  @param out: ponteiro onde o handler da rota será retornado
*/
int find_route(server_t* server, const char* path, http_method_t method, route_t** out);

#endif // __HTTP_SERVER_H