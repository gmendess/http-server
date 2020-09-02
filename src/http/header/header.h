#ifndef __HTTP_HEADER_H
#define __HTTP_HEADER_H

#include "../../util/hmap.h"

/*
  Estrutura que contém informações sobre os campos do header de uma requisição/resposta HTTP.
*/
typedef struct header_field {
  char* name;  // nome do campo
  char* value; // valor
} header_field_t;

/*
  Adiciona uma entry na hash map apontada por @header

  @param header: hash map de header_field_t
  @param field: nome de um campo do header
  @param value: conteúdo do campo do header de nome @field
*/
void add_header_field(hmap_t* header, const char* field, const char* value);

/*
  Analisa o header de uma requisição/resposta HTTP criando um header_field_t para cada campo e
  o adicionando em uma hash map.
  Retorna diferente de 0 em caso de erro

  @param header_lines: vetor de strings que contém cada linha do header
  @param len: quantidade de linha de @header_lines
  @param header: hash map que conterá cada campo do header da requisição
*/
int parse_header_lines(char** header_lines, int header_len, hmap_t* header);

/*
  Libera um header_field_t
  Essa função será usada como callback na função hmap_iterate, que percorrerá todas as
  entries do map, executando essa função abaixo.

  @param value: header_field_t a ter sua memória liberada
*/
void free_header_field(const char* _, void* value, void* __);

#endif // __HTTP_HEADER_H