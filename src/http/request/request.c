#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "request.h"
#include "../methods/methods.h"
#include "../../util/util.h"

/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_line_t.

  @param req_buffer: mensagem da requisição
  @param req_line: ponteiro p/ estrutura que conterá a request line
*/
void parse_request_line(char* req_buffer, request_line_t *const req_line) {
  char* token = NULL;

  token = strtok(req_buffer, " \r\n"); // método HTTP
  req_line->method = get_http_method(token);
  if(req_line->method.code == NONE)
    panic("parse_request_line: get_http_method", "método HTTP não suportado!");

  token = strtok(NULL, " \r\n"); // path
  if(token == NULL)
    panic("parse_request_line", "path não pode ser nulo!");
  req_line->path = token;

  token = strtok(NULL, " \r\n"); // versão do protocolo HTTP
  if(token == NULL)
    panic("parse_request_line", "versão do protocolo inválida!");
  req_line->version = token;

}

void parse_request(char* req_buffer, request_t* const req) {
  // adquire o body da requisição (req_buffer conterá apenas o header caso exista um body)
  char* body = get_request_body(req_buffer);
  req->body = body;

  // adquire individualmente cada linha do header, armazenando um ponteiro para
  // cada uma em header_lines
  int counter = char_counter(req_buffer, '\n');
  char** header_lines = NULL;
  counter = parse_lines(req_buffer, &header_lines, counter);

  char* req_line = *header_lines++;
  parse_request_line(req_line, &req->req_line);
  
  req->header = header_lines;
  req->header_counter = counter;

}

char* get_request_body(char* req_buffer) {
  char* body = NULL;
  body = strstr(req_buffer, "\r\n\r\n");
  if(body == NULL)
    panic("parse_request", "fim do header(CRLF + CRLF) não encontrado!");
  
  // body vazio
  if(body[4] == '\0')
    return NULL;
  else {
    *body = '\0'; // null char para separar o header do body
    body += 4;    // incrementa o body para pular os "\0\n\r\n"

    // cria e retorna uma cópia do body, pois este aponta para uma região em req_buffer
    return make_copy(body);
  }

}