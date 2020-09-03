#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include "status-codes/status_codes.h"
#include "../request/request.h"
#include "../header/header.h"
#include "../../errors/errors.h"

/*
  Estrutura que representa uma resposta HTTP
*/
typedef struct {
  int              clientfd; // socket do cliente
  http_status_code_t status; // status da resposta HTTP
  hmap_t             header; // hash map dos campos do header
} response_t;

/*
  Envia uma resposta HTTP para um cliente.

  @param resp: informações sobre status e cabeçalhos da resposta
  @param body: corpo da resposta, pode ser NULL
*/
int send_http_response(response_t* resp, const char* body);

/*
  Envia uma resposta padrão com base no status passado

  @param resp: informações sobre a resposta http
  @param status_code: status da resposta que identificará a mensagem padrão a ser
   enviada ao cliente
*/
int send_default(response_t* resp, http_status_code_t status_code);

#endif // __HTTP_RESPONSE_H