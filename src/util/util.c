#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
#include <arpa/inet.h>
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
char* make_copy(const char* buffer) {
  size_t len = strlen(buffer) + 1;
  
  char* copy = must_calloc(len, sizeof(char));

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
  if(lines == 0)
    lines = 1;

  *str_array = must_calloc(lines, sizeof(char*));
  
  char* token = strtok(buffer, "\r\n");
  int curr_line = 0;
  while(token) {
    if(lines == curr_line) {
      lines++;
      *str_array = realloc(*str_array, lines * sizeof(char*));
      if(*str_array == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
      }
    }

    (*str_array)[curr_line] = make_copy(token);
    curr_line++;
    token = strtok(NULL, "\r\n");
  }

  return curr_line;
}

/*
  Libera a memória de um vetor de strings retornado por parse_lines

  @param lines: vetor de strings que terá memória liberada
  @param len: tamanho do vetor @lines
*/
void free_lines(char** lines, int len) {
  for(int i = 0; i < len; i++)
    free(lines[i]);
  free(lines);
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

void sigint_handler(int sig) {
  write(2, "\nSIGINT recebido! O servidor será desligado!\n", 46);
}

/*
  Adquire o endereço IP e a porta de uma estrutura de endereço (sockaddr_in e sockaddr_in6)

  @param sa: estrutura do endereço
  @param port: ponteiro para inteiro que conterá a porta
  @param buffer: buffer que conterá o IP em formato legível
  @param len: tamanho de @buffer 
*/
void get_addr_and_port(struct sockaddr* sa, int* port, char* buffer, int len) {
  // IPv4
  if(sa->sa_family == AF_INET) {
    inet_ntop(sa->sa_family, &((struct sockaddr_in*) sa)->sin_addr, buffer, len);
    if(port)
      *port = ntohs( ((struct sockaddr_in*) sa)->sin_port );
  }
  else { // IPv6
    inet_ntop(sa->sa_family, &((struct sockaddr_in6*) sa)->sin6_addr, buffer, len);
    if(port)
      *port = ntohs( ((struct sockaddr_in6*) sa)->sin6_port );
  }
}

/*
  Retorna a data e hora atual com o fuso horário a partir do Meridiano de Greenwich.

  Como exemplo, o texto retornado é formatado da seguinte forma: "Thu, 20 Aug 2020 21:56:58 GMT",
  sem as aspas
*/
const char* gmt_date_now() {
  static char formated_date[30] = {0};

  time_t time_now = time(NULL);
  struct tm* t = gmtime(&time_now);

  // Thu, 20 Aug 2020 21:56:58 GMT
  strftime(formated_date, 30, "%a, %d %b %Y %T GMT", t);

  return formated_date;
}

/*
  Mesma coisa que um calloc, contudo encerra o programa caso ocorra erro ao alocar a 
  memória solicitada

  @param num_elements: número de elementos à serem alocadas
  @param size: tamanho de cada @num_elements
*/
void* must_calloc(size_t num_elements, size_t size) {
  void* ptr = calloc(num_elements, size);
  if(ptr == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}