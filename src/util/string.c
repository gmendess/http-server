#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string.h"
#include "util.h"

void string_init(string_t* str, size_t capacity) {
  str->capacity = capacity;
  str->length   = 0;
  str->buffer   = must_calloc(capacity, sizeof(char)); 
}

int string_append(string_t* str, const char* buffer) {
  if(!buffer)
    return 0;

  size_t buffer_len = strlen(buffer);

  // se o tamanho da nova string for maior que a capacidade do buffer, significa que é necessário
  // realocar mais memória
  if(str->length + buffer_len + 1 > str->capacity) {
    // dobra a capacidade e adiciono o tamanho do buffer + null char
    str->capacity = (str->capacity * 2) + buffer_len + 1;
    str->buffer = must_realloc(str->buffer, str->capacity);
  }

  memcpy(str->buffer + str->length, buffer, buffer_len + 1);
  str->length += buffer_len;
  return 0;
}

void string_destroy(string_t* str) {
  free(str->buffer);
  str->buffer   = NULL;
  str->capacity = 0;
  str->length   = 0;
}