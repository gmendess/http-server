#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "response.h"
#include "../../util/util.h"

/*
  Envia uma resposta HTTP para um cliente.

  @param resp: informações sobre status e cabeçalhos da resposta
  @param body: corpo da resposta, pode ser NULL
*/
int send_http_response(response_t* resp, const char* body) {
  char buf[32768] = {0};
  int offset = 0;

  offset += snprintf(&buf[offset], sizeof(buf) - offset, "HTTP/1.1 %d %s\r\n", resp->status, get_http_status_description(resp->status));

  header_field_t* aux = resp->header.field;
  while(aux) {
    offset += snprintf(&buf[offset], sizeof(buf) - offset, "%s: %s\r\n", aux->name, aux->value);
    aux = aux->next;
  }


  if(body) {
    offset += snprintf(&buf[offset], sizeof(buf) - offset, "Content-Length: %lu\r\n\r\n", strlen(body));
    offset += snprintf(&buf[offset], sizeof(buf) - offset, "%s", body);
  }
  else
    offset += snprintf(&buf[offset], sizeof(buf) - offset, "\r\n");

  send(resp->clientfd, buf, offset, 0);

  return offset;
}