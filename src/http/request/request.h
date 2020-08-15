#ifndef __HTTP_REQUEST_H
#define __HTTP_REQUEST_H

#include "../methods/methods.h"

/*
  Estrutura que contém informações sobre os campos de um header de uma requisição HTTP. Possui o
  membro 'next' para criar uma lista encadeada
*/
typedef struct request_field {
  char* name;            // nome do campo
  char* value;           // valor
  struct request_field* next; // próximo request_field_t
} request_field_t;

/*
  Estrutura que contém informações sobre a request line de uma requisição HTTP
*/
typedef struct request_line {
  http_method_t method;  // método HTTP (ex.: POST, GET, DELETE etc)
  char* path;            // caminho do recurso que está sendo requisitado (ex.: /admin/login/)
  char* version;         // versão do protocolo (ex.: HTTP/1.1)
} request_line_t;

/*
  Estrutura que contém informações sobre a requisição HTTP feita por um cliente
*/
typedef struct request {
  request_line_t    req_line;
  request_field_t*  header;
  char*  body;
} request_t;

/*
  Ponto de entrada para começar o parser da requisição

  @param req_buffer: mensagem completa da requisição
  @param req: estrutura que conterá cada informação da requisição
*/
void parse_request(char* req_buffer, request_t* const req);

/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_line_t.

  @param req_buffer: mensagem da requisição
  @param req_line: ponteiro p/ estrutura que conterá a request line
*/
void parse_request_line(char* req_buffer, request_line_t *const req_line);

/*
  Analisa o buffer da requisição em busca de um body, se houver, retorna a string que o represeta,
  caso contrário, retorna NULL

  Obs: se houver um body, req_buffer será modificado, colocando um null character após o término
  do header, separando o header do body

  @param req_buffer: buffer da requisição
*/
char* get_request_body(char* req_buffer);

/*
  Analisa o header da requisição criando um request_field_t para cada campo.

  @param header_lines: vetor de strings que contém cada linha do header da requisição
  @param len: quantidade de linha de @header_lines
  @param field: lista encadeada de request_field_t (cada campo do header)
*/
void parse_request_header(char** header_lines, int len, request_field_t** field);

#endif // __HTTP_REQUEST_H