#include <stdlib.h>
#include <stdio.h>

#ifndef __USE_POSIX
#define __USE_POSIX
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif

#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "util.h"

/*
  Exibe a mensagem de erro correspondente ao retorno de getaddrinfo e
  encerra o programa

  @param func_name: nome da função que deu erro
  @param err: retorno de getaddrinfo
*/
void net_panic(const char* func_name, const int err) {
  fprintf(stderr, "%s: %s\n", func_name, gai_strerror(err));
  exit(EXIT_FAILURE);
}

/*
  Exibe a mensagem de erro correspondente à errno e encerra o programa

  @param func_name: nome da função que deu erro
*/
void errno_panic(const char* func_name) {
  perror(func_name);
  exit(EXIT_FAILURE);
}

/*
  Exibe uma mensagem de erro que encerra o programa

  @param func_name: nome da função que deu erro
  @param msg: mensagem de erro
*/
void panic(const char* func_name, const char* msg) {
  fprintf(stderr, "%s: %s\n", func_name, msg);
  exit(EXIT_FAILURE);
}

/*
  Retorna a quantidade de vezes que um caractere aparece em uma string

  @param buffer: string que será analisada
  @param c: caractere a ser contado
*/
int char_counter(char* buffer, char c) {
  int counter = 0;
  for(; *buffer; buffer++)
    if(*buffer == c)
      counter++;
  return counter;
}

/*
  Cria e retorna uma cópia de um buffer

  @param buffer: buffer que será copiado
*/
char* make_copy(char* buffer) {
  size_t len = strlen(buffer) + 1;
  
  char* copy = calloc(len, sizeof(char));
  if(copy == NULL)
    errno_panic("make_copy: calloc");

  memcpy(copy, buffer, len);
  return copy;
}

/*
  Sepera o conteúdo de um buffer em várias linhas; retorna a quantidade de strings contidas em @str_array

  @param buffer: buffer de caracteres que sofrerá o parser de linha
  @param str_array: ponteiro para vetor de strings que conterá as linhas 'parseadas' de @buffer
  @param lines: uma "dica" de quantas linhas é possível que @buffer tenha (0 se não souber).
*/
int parse_lines(char* buffer, char*** str_array, int lines) {
  if(str_array == NULL)
    return -1;

  if(lines == 0)
    lines = 1;

  *str_array = calloc(lines, sizeof(char*));
  if(*str_array == NULL)
    errno_panic("parse_lines: calloc");
  
  char* token = strtok(buffer, "\r\n");
  int curr_line = 0;
  while(token) {
    if(lines == curr_line) {
      lines++;
      *str_array = realloc(*str_array, lines * sizeof(char*));
      if(*str_array == NULL)
        errno_panic("parse_lines: realloc");
    }

    (*str_array)[curr_line] = make_copy(token);
    curr_line++;
    token = strtok(NULL, "\r\n");
  }

  return curr_line;
}

/*
  Recebe o SIGCHLD do processo filho, evitando que ele se torne zumbi. O while evita 
  que SIGCHLDs sejam ignorados caso eles cheguem enquanto outro estiver sendo tratado. 
  Se waitpid retornar 0, significa que não há mais nenhum SIGCHLD a ser tratado nesse 
  momento. Verifica, também, se o processo filho foi terminado por um SIGSEGV.

  @param sig: número do sinal recebido
*/
void sigchld_handler(int sig) {
  int status = 0; // status retornado por waitpid
  while(waitpid(-1, &status, WNOHANG) > 0) {
    // verifica se o processo filho foi terminado por um SIGSEGV
    if(WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)
      write(2, "\nUm processo filho terminou devido SIGSEGV!\n", 44);
  }
}