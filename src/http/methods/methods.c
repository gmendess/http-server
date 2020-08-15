#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "methods.h"

/*
  Vetor de __http_method contendo todos os tipos de método HTTP
*/

/*
  Estrutura para ser usado no vetor 'methods'
  Contém um código do método http(ver methods.h) e o nome do método

  Coloquei essa estrutura aqui para não ficar visível
*/
struct __http_method {
  http_method_t code;
  char* name;
};

static const 
struct __http_method methods[] = {
  {GET,     "GET"},
  {POST,    "POST"},
  {PUT,     "PUT"},
  {DELETE,  "DELETE"},
  {HEAD,    "HEAD"},
  {OPTIONS, "OPTIONS"},
  {PATCH,   "PATCH"},
  {NONE,    NULL}
};

/*
  Adquire o identificador inteiro(http_method_t) da string passada em @method

  @param method: string que representa o método HTTP(ex.: "GET", "POST", "DELETE" etc)
*/
http_method_t get_http_method(const char* method) {
  int i = 0;
  while(1) {
    if(methods[i].code == NONE || strcmp(methods[i].name, method) == 0)
      break;
    i++;
  }
  
  return methods[i].code;
}

/*
  Adquire o nome do método HTTP a partir de seu identificador inteiro.

  ATENÇÃO: o ponteiro retornado não deve ter seu conteúdo alterado!

  @param method: identificador do método HTTP
*/
const char* get_http_method_name(http_method_t method) {
  return methods[method].name;
}
