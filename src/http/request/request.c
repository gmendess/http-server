#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "request.h"
#include "../methods/methods.h"

/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_t. Retorna um ponteiro para a próxima linha de req_msg 

  @param req_msg: mensagem da requisição
  @param req: ponteiro para um request_t que conterá informações da requisição
*/
char* parse_request_line(char* req_msg, request_t *const req) {
  char* token = NULL;

  token = strtok(req_msg, " \r\n");
  req->method = get_http_method(token);
  
  token = strtok(NULL, " \r\n");
  req->path = token;

  token = strtok(NULL, " \r\n");
  req->version = token;

  return 0;
}

