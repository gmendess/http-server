#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include "../methods/methods.h"

/*
  Estrutura que contém informações sobre a requisição HTTP feita por um cliente
*/
typedef struct request {
  http_method_t method;  // método HTTP (ex.: POST, GET, DELETE etc)
  char* path;            // caminho do recurso que está sendo requisitado (ex.: /admin/login/)
  char* version;         // versão do protocolo (ex.: HTTP/1.1)
} request_t;


/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_t. Retorna um ponteiro para a próxima linha de req_msg 

  @param req_msg: mensagem da requisição
*/
char* parse_request_line(char* req_msg, request_t *const req);

#endif // __HTTP_REQUEST_H