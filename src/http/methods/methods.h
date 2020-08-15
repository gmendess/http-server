#ifndef __HTTP_METHODS_H
#define __HTTP_METHODS_H

#include <stdlib.h>

typedef enum {
  NONE = 0, // não existe
  GET,
  POST,
  PUT,
  DELETE,
  HEAD,
  OPTIONS,
  PATCH
} http_method_code_t;

typedef struct {
  http_method_code_t code;
  char* name;
} http_method_t;

http_method_t get_http_method(const char* method);

#endif // __HTTP_METHODS_H