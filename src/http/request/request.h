#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include "methods/methods.h"
#include "../header/header.h"
#include "../../util/hmap.h"

/*
  Estrutura que contém a versão do protocolo HTTP
*/
typedef struct {
  int major;
  int minor;
} version_t;

/*
  Estrutura que contém informações sobre a request line de uma requisição HTTP
*/
typedef struct request_line {
  http_method_t method; // método HTTP (ex.: POST, GET, DELETE etc)
  char* path;           // caminho do recurso que está sendo requisitado (ex.: /admin/login/)
  version_t version;    // versão do protocolo (ex.: HTTP/1.1)
} request_line_t;

/*
  Estrutura que contém informações sobre a requisição HTTP feita por um cliente
*/
typedef struct request {
  request_line_t  req_line; // request line
  hmap_t header;            // hash map de cada campo do header
  char*  body;              // corpo da requisição (NULL se não houver)
} request_t;

/*
  Ponto de entrada para começar o parser da requisição.
  Retorna diferente de 0 em caso de erro

  @param req_buffer: mensagem completa da requisição
  @param req: estrutura que conterá cada informação da requisição
*/
int parse_request(char* req_buffer, request_t* const req);

/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_line_t.
  Retorna diferente de 0 em caso de erro.

  @param req_buffer: mensagem da requisição
  @param req_line: ponteiro p/ estrutura que conterá a request line
*/
int parse_request_line(char* req_buffer, request_line_t *const req_line);

/*
  Analisa o buffer da requisição em busca de um body, se houver, retorna 0 e preenche @body_out com a 
  string que o representa, caso contrário, retorna diferente de 0.

  Obs: se houver um body, req_buffer será modificado, colocando um null character após o término
  do header, separando o header do body

  @param req_buffer: buffer da requisição
  @param body_out: ponteiro para a string contendo o body retornado
*/
int get_request_body(char* req_buffer, char** body_out);

/*
  Libera a memória de um request_t

  @param req: request_t que terá memória de seus membros liberados
*/
void free_request(request_t* req);

/*
  Faz parse da versão do protocolo HTTP

  @param buffer: buffer que será analisado para adquirir a versão do protocolo
  @param version: ponteiro para estrutura que conterá a versão do protocolo após a análise
*/
int parse_version(char* buffer, version_t* version);

#endif // __HTTP_REQUEST_H