#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "request.h"
#include "../methods/methods.h"
#include "../../util/util.h"

/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_t. Retorna um ponteiro para a próxima linha de req_msg 

  @param req_msg: mensagem da requisição
  @param req: ponteiro para um request_t que conterá informações da requisição
*/
char* parse_request_line(char* req_msg, request_t *const req) {
  char* token = NULL;

  token = strtok(req_msg, " \r\n"); // método HTTP
  req->method = get_http_method(token);
  if(req->method.code == NONE)
    panic("parse_request_line: get_http_method", "método HTTP não suportado!");

  token = strtok(NULL, " \r\n"); // path
  if(token == NULL)
    panic("parse_request_line", "path não pode ser nulo!");
  req->path = token;

  token = strtok(NULL, " \r\n"); // versão do protocolo HTTP
  if(token == NULL)
    panic("parse_request_line", "versão do protocolo inválida!");
  req->version = token;

  return 0;
}

