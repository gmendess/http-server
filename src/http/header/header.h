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
  Estrutura usada para representar uma lista encadeada de campos de cabeçalho. Criei ela para
  facilitar o uso de funções que manipular uma lista de header_field_t
*/
typedef struct {
  header_field_t* field; // primeiro campo do cabeçalho
} header_t;

/*
  Adiciona um nó à lista encadeada apontada por @header

  @param header: lista encadeada de header_field_t
  @param field: nome de um campo do header
  @param value: conteúdo do campo do header de nome @field
*/
void add_header_field(header_t* header, const char* field, const char* value);

/*
  Analisa o header de uma requisição/resposta HTTP criando um header_field_t para cada campo.

  @param header_lines: vetor de strings que contém cada linha do header
  @param len: quantidade de linha de @header_lines
  @param field: lista encadeada de header_field_t (cada campo do header)
*/
void parse_header_lines(char** header_lines, int header_len, header_t* header);

/*
  Libera uma lista encadeada de header_field_t

  @param header: estrutura que representa a lista encadeada
*/
void free_header(header_t* header);

#endif // __HTTP_HEADER_H