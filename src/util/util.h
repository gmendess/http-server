#ifndef __HTTP_UTIL_H
#define __HTTP_UTIL_H

#include <arpa/inet.h>

/*
  Exibe a mensagem de erro correspondente ao retorno de getaddrinfo e
  encerra o programa

  @param func_name: nome da função que deu erro
  @param err: retorno de getaddrinfo
*/
void net_panic(const char* func_name, const int err);

/*
  Retorna a quantidade de vezes que um caractere aparece em uma string

  @param buffer: string que será analisada
  @param c: caractere a ser contado
*/
int char_counter(char* buffer, char c);

/*
  Cria e retorna uma cópia de um buffer

  @param buffer: buffer que será copiado
*/
char* make_copy(const char* buffer);

/*
  Sepera o conteúdo de um buffer em várias linhas; retorna a quantidade de strings contidas em @str_array

  @param buffer: buffer de caracteres que sofrerá o parser de linha
  @param str_array: ponteiro para vetor de strings que conterá as linhas 'parseadas' de @buffer
  @param lines: uma "dica" de quantas linhas é possível que @buffer tenha (0 se não souber).
*/
int parse_lines(char* buffer, char*** str_array, int lines);

/*
  Recebe o SIGCHLD do processo filho, evitando que ele se torne zumbi.
  Verifica, também, se o processo filho foi terminado por um SIGSEGV.

  @param sig: número do sinal recebido
*/
void sigchld_handler(int sig);

/*
  Adquire o endereço e a porta de uma estrutura de endereço (sockaddr_in e sockaddr_in6)

  @param sa: estrutura do endereço
  @param port: ponteiro para inteiro que conterá a porta
  @param buffer: buffer que conterá o IP em formato legível
  @param len: tamanho de @buffer 
*/
void get_addr_and_port(struct sockaddr* sa, int* port, char* buffer, int len);

/*
  Libera a memória de um vetor de strings retornado por parse_lines

  @param lines: vetor de strings que terá memória liberada
  @param len: tamanho do vetor @lines
*/
void free_lines(char** lines, int len);

/*
  Retorna a data e hora atual com o fuso horário a partir do Meridiano de Greenwich.

  Como exemplo, o texto retornado é formatado da seguinte forma: "Thu, 20 Aug 2020 21:56:58 GMT",
  sem as aspas
*/
const char* gmt_date_now();

/*
  Mesma coisa que um calloc, contudo encerra o programa caso ocorra erro ao alocar a 
  memória solicitada

  @param num_elements: número de elementos à serem alocadas
  @param size: tamanho de cada @num_elements
*/
void* must_calloc(size_t num_elements, size_t size);

#endif // __HTTP_UTIL_H