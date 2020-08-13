#ifndef __HTTP_UTIL_H
#define __HTTP_UTIL_H

/*
  Exibe a mensagem de erro correspondente ao retorno de getaddrinfo e
  encerra o programa

  @param func_name: nome da função que deu erro
  @param err: retorno de getaddrinfo
*/
void net_panic(const char* func_name, const int err);

/*
  Exibe a mensagem de erro correspondente à errno e encerra o programa

  @param func_name: nome da função que deu erro
*/
void errno_panic(const char* func_name);

/*
  Retorna a quantidade de vezes que um caractere aparece em uma string

  @param buffer: string que será analisada
  @param c: caractere a ser contado
*/
int char_counter(char* buffer, char c);

/*
  Cria e retorna uma cópia de um buffer

  @param buffer: buffer que será copiada
*/
char* make_copy(char* buffer);

#endif // __HTTP_UTIL_H