#ifndef __HTTP_HEADER_H
#define __HTTP_HEADER_H

/*
  Estrutura que contém informações sobre os campos do header de uma requisição/resposta HTTP. Possui o
  membro 'next' para criar uma lista encadeada
*/
typedef struct header_field {
  char* name;  // nome do campo
  char* value; // valor
  struct header_field* next; // próximo header_field_t
} header_field_t;

/*
  Analisa o header de uma requisição/resposta HTTP criando um header_field_t para cada campo.

  @param header_lines: vetor de strings que contém cada linha do header
  @param len: quantidade de linha de @header_lines
  @param field: lista encadeada de header_field_t (cada campo do header)
*/
void parse_header_lines(char** header_lines, int header_len, header_field_t** field);

#endif // __HTTP_HEADER_H