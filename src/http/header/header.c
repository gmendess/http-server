#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../util/util.h"
#include "../../errors/errors.h"
#include "header.h"

/*
  Adiciona um nó à lista encadeada apontada por @header

  @param header: lista encadeada de header_field_t
  @param field: nome de um campo do header
  @param value: conteúdo do campo do header de nome @field
*/
void add_header_field(header_t* header, const char* field, const char* value) {
  header_field_t* new = must_calloc(1, sizeof(*new));

  new->name  = make_copy(field);
  new->value = make_copy(value);
  new->next  = header->field;

  header->field = new;
}

/*
  Analisa o header de uma requisição/resposta HTTP criando um header_field_t para cada campo.

  @param header_lines: vetor de strings que contém cada linha do header
  @param len: quantidade de linha de @header_lines
  @param header: lista encadeada de header_field_t (cada campo do header)
*/
int parse_header_lines(char** header_lines, int header_len, header_t* header) {
  char* field = NULL,
      * value = NULL;

  for(int i = 0; i < header_len; i++) {
    field = strtok(header_lines[i], ":");
    value = strtok(NULL, "\0");

    if(!field || !value)
      return ERR_HEADER;

    while(*value == ' ')
      ++value;

    add_header_field(header, field, value);
  }

  return 0;
}

/*
  Libera uma lista encadeada de header_field_t

  @param header: estrutura que representa a lista encadeada
*/
void free_header(header_t* header) {
  while(header->field) {
    header_field_t* save_next = header->field->next;
    free(header->field->name);
    free(header->field->value);
    free(header->field);
    header->field = save_next;
  }
  header->field = NULL;
}