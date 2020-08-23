#ifndef __HTTP_ERRORS_H
#define __HTTP_ERRORS_H

// Header inválido
#define ERR_HEADER   1

// Request-line inválido
#define ERR_REQLINE  2

// Método HTTP inválido
#define ERR_HTTPMET  3

// Não foi possível atribuir nenhum endereço ao servidor
#define ERR_BIND     4

// Erro ao adquirir lista de endereços (getaddrinfo)
#define ERR_ADDRLIST 5

// Erro ao colocar servidor para escutar por requisições de conexão
#define ERR_LISTEN   6

// Rota inválida
#define ERR_ROUTE    7

// Servidor inválido
#define ERR_SERVER   8

void http_error(const char* func_name, int err);

#endif // __HTTP_ERRORS_H