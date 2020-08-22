#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include "status-codes/status_codes.h"
#include "../request/request.h"
#include "../header/header.h"

/*
  Estrutura que representa uma resposta HTTP
*/
typedef struct {
  int              clientfd; // socket do cliente
  http_status_code_t status; // status da resposta HTTP
  header_t           header; // lista encadeada de campos do header
} response_t;

/*
  Envia uma resposta HTTP para um cliente.

  @param resp: informações sobre status e cabeçalhos da resposta
  @param body: corpo da resposta, pode ser NULL
*/
int send_http_response(response_t* resp, const char* body);

int send_default_200(response_t* resp);

int send_default_404(response_t* resp);

int send_default_405(response_t* resp);

#endif // __HTTP_RESPONSE_H