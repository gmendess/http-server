#include <stdlib.h>
#include "status_codes.h" 

// [1xx] Códigos de informações
static const 
char* info_codes[] = {
  "Continue",           // 100
  "Switching Protocol", // 101
  "Processing",         // 102
  "Early Hints"         // 103
};

// [2xx] Códigos de sucesso
static const
char* success_codes[] = {
  "OK",              // 200
  "Created",         // 201
  "Accepted",        // 202
  "Non-Authoritative Information", // 203
  "No Content",      // 204
  "Reset Content",   // 205
  "Partial Content", // 206
  "Multi-Status",    // 207
  "Multi-Status",    // 208
  "IM Used"          // 226
};

// [3xx] Códigos de redirecionamento
static const
char* redirection_codes[] = {
  "Multiple Choice",    // 300
  "Moved Permanently",  // 301
  "Found",              // 302
  "See Other",          // 303
  "Not Modified",       // 304
  "Use Proxy",          // 305
  NULL,                 // 306 (unused)
  "Temporary Redirect", // 307
  "Permanent Redirect", // 308
};

// [4xx] Códigos de erro do cliente
static const
char* client_error_codes[] = {
  "Bad Request",                     // 400
  "Unauthorized",                    // 401
  "Payment Required",                // 402
  "Forbidden",                       // 403
  "Not Found",                       // 404
  "Method Not Allowed",              // 405
  "Not Acceptable",                  // 406
  "Proxy Authentication Required",   // 407
  "Request Timeout",                 // 408
  "Conflict",                        // 409
  "Gone",                            // 410
  "Length Required",                 // 411
  "Precondition Failed",             // 412
  "Payload Too Large",               // 413
  "URI Too Long",                    // 414
  "Unsupported Media Type",          // 415
  "Requested Range Not Satisfiable", // 416
  "Expectation Failed",              // 417
  "I'm a teapot",                    // 418
  NULL,
  NULL,
  "Misdirected Request",             // 421
  "Unprocessable Entity",            // 422
  "Locked",                          // 423
  "Failed Dependency",               // 424
  "Too Early",                       // 425
  "Upgrade Required",                // 426
  NULL,
  "Precondition Required",           // 428
  "Too Many Requests",               // 429
  NULL,
  "Request Header Fields Too Large", // 431
  "Unavailable For Legal Reasons",   // 451
};

// [5xx] Códigos de erro do servidor
static const
char* server_error_codes[] = {
  "Internal Server Error",           // 500
  "Not Implemented",                 // 501
  "Bad Gateway",                     // 502
  "Service Unavailable",             // 503
  "Gateway Timeout",                 // 504
  "HTTP Version Not Supported",      // 505
  "Variant Also Negotiates",         // 506
  "Insufficient Storage",            // 507
  "Loop Detected",                   // 508
  "Bandwidth Limit Exceeded",        // 509
  "Not Extended",                    // 510
  "Network Authentication Required"  // 511
};

const char* get_info_code_description(int index) {
  if(index >= 4)
    return NULL;

  return info_codes[index];
}

const char* get_success_code_description(int index) {
  if(index == 26)
    return success_codes[9]; // 226

  if(index >= 9)
    return NULL;

  return success_codes[index];
}

const char* get_redirection_code_description(int index) {
  if(index >= 9)
    return NULL;

  return redirection_codes[index];
}

const char* get_client_error_code_description(int index) {
  if(index == 51)
    return client_error_codes[32]; // 451

  if(index >= 32)
    return NULL;

  return client_error_codes[index];
}

const char* get_server_error_code_description(int index) {
  if(index >= 12)
    return NULL;

  return server_error_codes[index];
}

/*
  Adquire a descrição de um código de status HTTP

  ATENÇÃO: a string retornada não deve ser modificada!

  @param status_code: código de status HTTP(ver http_status_code_t)
*/
const char* get_http_status_description(http_status_code_t status_code) {
  if(status_code == 0)
    return NULL;

  int status_family = status_code / 100;

  int index = status_code % 100;
  if(index < 0)
    return NULL;

  switch(status_family) {
    case 1:  // 1xx (Informacional)
      return get_info_code_description(index);            break;
    case 2:  // 2xx (Sucesso)
      return get_success_code_description(index);         break;
    case 3:  // 3xx (Redirecionamento)
      return get_redirection_code_description(index);     break;
    case 4:  // 4xx (Erro no cliente)
      return get_client_error_code_description(index);    break;
    case 5:  // 5xx (Erro no servidor)
      return get_server_error_code_description(index);    break;
    default: // família de código inválida
      return NULL;
  }
}