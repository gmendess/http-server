#include <stdlib.h>
#include <stdio.h>
#include "response.h"
#include "../../util/util.h"

void add_header(response_t* resp, const char* field, const char* value) {
  header_field_t* aux  = resp->header;
  header_field_t* prev = NULL;
  while(aux) {
    prev = aux;
    aux  = aux->next;
  }

  header_field_t* new = calloc(1, sizeof(header_field_t));
  if(!new)
    errno_panic("add_header: malloc");
  new->name  = make_copy(field);
  new->value = make_copy(value);

  if(prev == NULL)
    resp->header = new;
  else
    prev->next = new;
}
