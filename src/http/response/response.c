#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "response.h"
#include "../../util/util.h"
#include "../../util/string.h"

static int send_all(int client_fd, const char* buffer, size_t len) {
  // ponteiro para o buffer. Esse ponteiro será deslocado
  const char* ptr = buffer;

  // enquanto o tamanho do buffer for maior que zero, significa que ainda há bytes para serem enviados
  while(len > 0) {
    ssize_t bytes_sent = send(client_fd, ptr, len, MSG_NOSIGNAL);
    if(bytes_sent == -1)
      return ERR_SEND;

    ptr += bytes_sent; // desloca ptr pela quantidade de bytes enviados
    len -= bytes_sent; // decrementa o tamanho do buffer
  }
  return 0;
}

/*
  Função estática auxiliar que montará o header da resposta HTTP. Essa função será 
  usada como callback na chamada de hmap_iterate.

  @param key: chave de um bucket do map (nome do field do header)
  @param value: valor da chave correspondente (header_field_t)
  @param args: ponteiro para string_t
*/
static void construct_resp_header(const char* key, void* value, void* args) {
  header_field_t* field = value;
  string_t* r = args;

  char buffer[2048] = {0};
  snprintf(buffer, sizeof(buffer), "%s: %s\r\n", key, field->value);
  string_append(r, buffer);
}

/*
  Envia uma resposta HTTP para um cliente.

  @param resp: informações sobre status e cabeçalhos da resposta
  @param body: corpo da resposta, pode ser NULL
*/
int send_http_response(response_t* resp, const char* body) {
  string_t r;
  string_init(&r, 1024);

  char buffer[2048] = {0};

  // Send-line da resposta (Versão, status e descrição do status)
  snprintf(buffer, sizeof(buffer), "HTTP/1.1 %d %s\r\n", resp->status, get_http_status_description(resp->status));
  string_append(&r, buffer);

  //adiciona ao header da resposta todos os campos e valores existentes no map
  hmap_iterate(&resp->header, construct_resp_header, (void*) &r);

  // verifica se o status da resposta inclui um body
  if(include_body(resp->status)) {
    // Content-Length e body da resposta
    snprintf(buffer, sizeof(buffer), "Content-Length: %lu\r\n\r\n", strlen(body));
    string_append(&r, buffer);
    string_append(&r, body);
  }

  send_all(resp->clientfd, r.buffer, r.length);
  string_destroy(&r);

  return 0;
}

/*
  Envia uma resposta padrão com base no status passado

  @param resp: informações sobre a resposta http
  @param status_code: status da resposta que identificará a mensagem padrão a ser
   enviada ao cliente
*/
int send_default(response_t* resp, http_status_code_t status_code) {
  add_header_field(&resp->header, "Content-Type", "text/plain");
  resp->status = status_code;
  return send_http_response(resp, get_http_status_description(status_code));
}