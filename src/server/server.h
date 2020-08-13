#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

/*
  Fila de espera do socket; usado na syscall 'listen'
*/
#define BACKLOG 5

/*
  Cria um novo servidor http e retorna file descriptor

  @param addr: endereço do servidor 
  @para port: serviço (porta)
*/
int new_http_server(const char* addr, const char* port);

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.

  @param listener: file descriptor do servidor; retornado por new_http_server 
*/
void start_listening(int listener);

#endif // __HTTP_SERVER_H