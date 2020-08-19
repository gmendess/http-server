#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include "status-codes/status_codes.h"
#include "../request/request.h"
#include "../header/header.h"

typedef struct {
  char* http_version;        // versão do protocolo (evitar mexer)
  http_status_code_t status; // código de status da resposta (ver status_codes.h)
} status_line_t;

typedef struct {
  int clientfd;              // socket do cliente
  status_line_t status_line; // status-line de uma resposta HTTP
  header_field_t* header;    // lista encadeada de campos do header
} response_t;

void add_header(response_t* resp, const char* field, const char* value);

#endif // __HTTP_RESPONSE_H