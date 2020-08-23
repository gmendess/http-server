#include <stdlib.h>
#include <stdio.h>

// Descrição de cada erro definido em errors.h
static const 
char* errors_str[] = {
  "sucesso",
  "header da requisição HTTP inválido",
  "request-line da requisição HTTP inválido",
  "método HTTP inválido",
  "não foi possível atribuir nenhum endereço ao servidor",
  "não foi possível adquirir lista de endereços por getaddrinfo",
  "não foi possível colocar o servidor para escutar por conexões",
  "rota informada não é válida",
  "servidor informado não é válido"
};

void http_error(const char* func_name, int err) {
  fprintf(stderr, "%s: %s\n", func_name, errors_str[err]);
}