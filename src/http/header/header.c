#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../util/util.h"
#include "header.h"

/*
  Analisa o header de uma requisição/resposta HTTP criando um header_field_t para cada campo.

  @param header_lines: vetor de strings que contém cada linha do header
  @param len: quantidade de linha de @header_lines
  @param field: lista encadeada de header_field_t (cada campo do header)
*/
void parse_header_lines(char** header_lines, int header_len, header_field_t** field) {
  if(header_len == 0)
    panic("parse_header_lines", "tamanho do header não pode ser zero!");

  char* token;
  for(int i = 0; i < header_len; i++) {
    // aloca memória para um field
    *field = calloc(1, sizeof(header_field_t));
    if(field == NULL)
      panic("parse_header_lines", "falha ao alocar memória para header_field_t");
    
    // parser para pegar o nome do field
    token = strtok(header_lines[i], ":");
    if(token == NULL)
      panic("parse_header_lines", "nome do campo do header não encontrado!");
    (*field)->name = make_copy(token);
    
    // parser para pegar o valor do field
    token = strtok(NULL, ":");
    if(token == NULL)
      panic("parse_header_lines", "valor do campo do header não encontrado!");
    if(*token == ' ')
      ++token;
    (*field)->value = make_copy(token);

    // field aponta para o endereço do próximo item da lista
    field = &(*field)->next;
  }
}