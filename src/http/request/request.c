#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "request.h"
#include "methods/methods.h"
#include "../../util/util.h"
#include "../../errors/errors.h"
#include "../header/header.h"

/*
  Libera a memória de um request_t

  @param req: request_t que terá memória de seus membros liberados
*/
void free_request(request_t* req) {
  free(req->body);
  free(req->req_line.path);
  free(req->req_line.version);
  free_header(&req->header);
}

/*
  Faz parse da Request-line de uma requisição http e salva as informações em
  uma request_line_t.
  Retorna diferente de 0 em caso de erro.

  @param req_buffer: mensagem da requisição
  @param req_line: ponteiro p/ estrutura que conterá a request line
*/
int parse_request_line(char* req_buffer, request_line_t *const req_line) {
  char* token = NULL;

  token = strtok(req_buffer, " \r\n"); // método HTTP
  req_line->method = get_http_method(token);
  if(req_line->method == NONE)
    return ERR_HTTPMET; // método HTTP inválido

  token = strtok(NULL, " \r\n"); // path
  if(token == NULL)
    return ERR_REQLINE; // req-line inválido, path não encontrado
  req_line->path = make_copy(token);

  token = strtok(NULL, " \r\n"); // versão do protocolo HTTP
  if(token == NULL)
    return ERR_REQLINE; // req-line inválido, versão não encontrada

  req_line->version = make_copy(token);
  return 0;
}

/*
  Ponto de entrada para começar o parser da requisição.
  Retorna diferente de 0 em caso de erro

  @param req_buffer: mensagem completa da requisição
  @param req: estrutura que conterá cada informação da requisição
*/
int parse_request(char* req_buffer, request_t* const req) {
  int err = 0; // variável para controle de erros

  // adquire o body da requisição (req_buffer passará a conter apenas o header caso exista um body)
  if( (err = get_request_body(req_buffer, &req->body)) != 0)
    return err;

  // quantidade de 'quebra linha' em req_buffer para servir como dica para parse_lines
  int counter = char_counter(req_buffer, '\n');

  // adquire individualmente cada linha do header, armazenando um ponteiro para
  // cada uma em header_lines (request line incluido)
  char** header_lines = NULL;
  counter = parse_lines(req_buffer, &header_lines, counter);

  char* req_line = header_lines[0];
  if( (err = parse_request_line(req_line, &req->req_line)) != 0 )
    return err;

  // faz parse em header_lines populando a lista encadeada em req->header
  err = parse_header_lines(header_lines + 1, counter - 1, &req->header);

  // libera memória do vetor de strings
  free_lines(header_lines, counter);

  return err;
}

/*
  Analisa o buffer da requisição em busca de um body, se houver, preenche @body_out com a string que 
  o representa e retorna 0, caso contrário, retorna -1.

  Obs: se houver um body, req_buffer será modificado, colocando um null character após o término
  do header, separando o header do body

  @param req_buffer: buffer da requisição
  @param body: ponteiro para a string contendo o body retornado
*/
int get_request_body(char* req_buffer, char** body_out) {
  char* body = NULL;
  body = strstr(req_buffer, "\r\n\r\n");
  if(body == NULL)
    return ERR_HEADER; // fim do header não encontrado

  // body vazio
  if(body[4] == '\0')
    *body_out = NULL;
  else {
    *body = '\0'; // null char para separar o header do body
    body += 4;    // incrementa o body para pular os "\0\n\r\n"

    // cria uma cópia do body, pois este aponta para uma região em req_buffer
    *body_out = make_copy(body);
  }

  return 0; // sucesso
}