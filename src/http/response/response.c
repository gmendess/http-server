#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "response.h"
#include "../../util/util.h"

static int send_all(int client_fd, const char* buffer, size_t len) {
  // ponteiro para o buffer. Esse ponteiro será deslocado
  const char* ptr = buffer;

  // enquanto o tamanho do buffer for maior que zero, significa que ainda há bytes para serem enviados
  while(len > 0) {
    int bytes_sent = send(client_fd, ptr, len, MSG_NOSIGNAL);
    if(bytes_sent == -1)
      return ERR_SEND;

    ptr += bytes_sent; // desloca ptr pela quantidade de bytes enviados
    len -= bytes_sent; // decrementa o tamanho do buffer
  }
  return 0;
}

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

  return send_all(resp->clientfd, buf, offset);
}

int send_default_200(response_t* resp) {
  char resp_200[256];

  snprintf(resp_200, sizeof(resp_200), 
    "HTTP/1.1 200 OK\r\n"          \
    "Date: %s\r\n"                 \
    "Content-Type: text/plain\r\n" \
    "Content-Length: 6\r\n\r\n"    \
    "200 OK",
    gmt_date_now()
  );

  return send(resp->clientfd, resp_200, strlen(resp_200), 0);
}

int send_default_404(response_t* resp) {
  char resp_404[256];

  snprintf(resp_404, sizeof(resp_404), 
    "HTTP/1.1 404 Not Found\r\n"    \
    "Date: %s\r\n"                  \
    "Content-Type: text/plain\r\n"  \
    "Content-Length: 13\r\n\r\n"    \
    "404 Not Found",
    gmt_date_now()
  );

  return send(resp->clientfd, resp_404, strlen(resp_404), 0);
}

int send_default_405(response_t* resp) {
  char resp_405[256];

  snprintf(resp_405, sizeof(resp_405), 
    "HTTP/1.1 405 Method Not Allowed\r\n"          \
    "Date: %s\r\n"                 \
    "Content-Type: text/plain\r\n" \
    "Content-Length: 22\r\n\r\n"    \
    "405 Method Not Allowed",
    gmt_date_now()
  );

  return send(resp->clientfd, resp_405, strlen(resp_405), 0);
}