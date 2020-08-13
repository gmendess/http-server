#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "methods.h"

/*
  Vetor de http_method_t contendo (quase) todos os tipos de método HTTP
*/
static const 
http_method_t methods[] = {
  {GET, "GET"},
  {POST, "POST"},
  {DELETE, "DELETE"},
  {PUT, "PUT"},
  {NONE, NULL}
};

http_method_t get_http_method(const char* method) {
  int i = 0;
  while(1) {
    if(methods[i].code == NONE || strcmp(methods[i].name, method) == 0)
      break;
    i++;
  }
  
  return methods[i];
}
