#ifndef __HTTP_METHODS_H
#define __HTTP_METHODS_H

#include <stdlib.h>

typedef enum {
  GET,
  POST,
  PUT,
  DELETE,
  HEAD,
  OPTIONS,
  PATCH,
  NONE
} http_method_t;

/*
  Adquire o identificador inteiro(http_method_t) da string passada em @method

  @param method: string que representa o método HTTP(ex.: "GET", "POST", "DELETE" etc)
*/
http_method_t get_http_method(const char* method);

/*
  Adquire o nome do método HTTP a partir de seu identificador inteiro.
  ATENÇÃO: o ponteiro retornado não deve ter seu conteúdo alterado!

  @param method: identificador do método HTTP
*/
const char* get_http_method_name(http_method_t method);

#endif // __HTTP_METHODS_H