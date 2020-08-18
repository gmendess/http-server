#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "request.h"
#include "methods/methods.h"
#include "../../util/util.h"
#include "../header/header.h"

/*
  Libera a memória de um request_t

  @param req: request_t que terá memória de seus membros liberados
*/
void free_request(request_t* req) {
  free(req->body);
  free(req->req_line.path);
  free(req->req_line.version);

  header_field_t* aux = req->header;
  header_field_t* save_next = NULL;
  while(aux) {
    save_next = aux->next;
    free(aux->name);
    free(aux->value);
    free(aux);
    aux = save_next;
  }
}

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
  if(req_line->method == NONE)
    panic("parse_request_line: get_http_method", "método HTTP não suportado!");

  token = strtok(NULL, " \r\n"); // path
  if(token == NULL)
    panic("parse_request_line", "path não pode ser nulo!");
  req_line->path = make_copy(token);

  token = strtok(NULL, " \r\n"); // versão do protocolo HTTP
  if(token == NULL)
    panic("parse_request_line", "versão do protocolo inválida!");
  req_line->version = make_copy(token);

}

/*
  Ponto de entrada para começar o parser da requisição

  @param req_buffer: mensagem completa da requisição
  @param req: estrutura que conterá cada informação da requisição
*/
void parse_request(char* req_buffer, request_t* const req) {
  // adquire o body da requisição (req_buffer passará a conter apenas o header caso exista um body)
  char* body = get_request_body(req_buffer);
  req->body = body;

  // adquire individualmente cada linha do header, armazenando um ponteiro para
  // cada uma em header_lines (request line incluido)
  int counter = char_counter(req_buffer, '\n'); // quantidade de '\n' em req_buffer para servir como dica para parse_lines
  char** header_lines = NULL;
  counter = parse_lines(req_buffer, &header_lines, counter);

  char* req_line = header_lines[0];
  parse_request_line(req_line, &req->req_line);
  
  // faz parse em header_lines populando a lista encadeada em req->header
  parse_header_lines(header_lines + 1, counter - 1, &req->header);

  // libera memória do vetor de strings
  free_lines(header_lines, counter);
}

/*
  Analisa o buffer da requisição em busca de um body, se houver, retorna a string que o represeta,
  caso contrário, retorna NULL

  Obs: se houver um body, req_buffer será modificado, colocando um null character após o término
  do header, separando o header do body

  @param req_buffer: buffer da requisição
*/
char* get_request_body(char* req_buffer) {
  char* body = NULL;
  body = strstr(req_buffer, "\r\n\r\n");
  if(body == NULL)
    panic("get_request_body", "fim do header(CRLF + CRLF) não encontrado!");
  
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