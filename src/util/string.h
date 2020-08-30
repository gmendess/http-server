#ifndef __STRING_UTIL_H
#define __STRING_UTIL_H

#include <stdlib.h>

typedef struct {
  char* buffer;
  size_t length;
  size_t capacity;
} string_t;

void string_init(string_t* str, size_t capacity);
int  string_append(string_t* str, const char* buffer);
void string_destroy(string_t* str);

#endif // __STRING_UTIL_H