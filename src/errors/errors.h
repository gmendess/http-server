#ifndef __HTTP_ERRORS_H
#define __HTTP_ERRORS_H

// Header inválido
#define ERR_HEADER  1000

// Request-line inválido
#define ERR_REQLINE 1001

// Método HTTP inválido
#define ERR_HTTPMET 1002

// Não foi possível atribuir nenhum endereço ao servidor
#define ERR_BIND    1003

#endif // __HTTP_ERRORS_H