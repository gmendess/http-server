#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../util/util.h"
#include "../../util/hmap.h"
#include "../../errors/errors.h"
#include "header.h"

/*
  Adiciona uma entry na hash map apontada por @header

  @param header: hash map de header_field_t
  @param field: nome de um campo do header
  @param value: conteúdo do campo do header de nome @field
*/
void add_header_field(hmap_t* header, const char* field, const char* value) {
  header_field_t* new = must_calloc(1, sizeof(*new));

  new->name  = make_copy(field);
  new->value = make_copy(value);

  hmap_set(header, new->name, new);
}

/*
  Analisa o header de uma requisição/resposta HTTP criando um header_field_t para cada campo e
  o adicionando em uma hash map.
  Retorna diferente de 0 em caso de erro

  @param header_lines: vetor de strings que contém cada linha do header
  @param len: quantidade de linha de @header_lines
  @param header: hash map que conterá cada campo do header da requisição
*/
int parse_header_lines(char** header_lines, int header_len, hmap_t* header) {
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
  Libera um header_field_t
  Essa função será usada como callback na função hmap_iterate, que percorrerá todas as
  entries do map, executando essa função abaixo.

  @param value: header_field_t a ter sua memória liberada
*/
void free_header_field(const char* _, void* value, void* __) {
  header_field_t* field = value;
  free(field->name);
  free(field->value);
  free(field);
}