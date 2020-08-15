#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

#include "../http/request/request.h"
#include "../http/methods/methods.h"

/*
  Fila de espera do socket; usado na syscall 'listen'
*/
#define BACKLOG 5

// função handler para um rota 
typedef void(*route_handler_t)(int, request_t*);

typedef struct __route_t {
  const char* path;           // caminho especificado na requisição
  http_method_code_t m_code;  // código do método (ver methods.h)
  route_handler_t    handler; // handler da rota
  struct __route_t*  next;    // próxima rota (NULL = fim da lista)
} route_t;

typedef struct {
  int listener;    // file descriptor do servidor
  char addr[INET6_ADDRSTRLEN]; // endereço do servidor
  int  port;       // porta do servidor
  route_t* route;  // lista encadeada das rotas do servidor
} server_t;

/*
  Cria um novo servidor http

  @param server: estrutura que conterá informações do servidor
  @param addr: endereço do servidor 
  @para port: serviço (porta)
*/
void new_http_server(server_t* server, const char* addr, const char* port);

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.

  @param server: estrutura server_t contendo informações do servidor a ser iniciado 
*/
void start_listening(server_t* server);

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
                  http_method_code_t m_code);

#endif // __HTTP_SERVER_H