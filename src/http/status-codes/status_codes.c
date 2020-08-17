#include <stdlib.h>
#include "status_codes.h" 

// [1xx] Códigos de informações
static const 
char* info_codes[] = {
  "Continue",           // 100
  "Switching Protocol", // 101
  "Processing",         // 102
  "Early Hints"         // 103
};

// [2xx] Códigos de sucesso
static const
char* success_codes[] = {
  "OK",              // 200
  "Created",         // 201
  "Accepted",        // 202
  "Non-Authoritative Information", // 203
  "No Content",      // 204
  "Reset Content",   // 205
  "Partial Content", // 206
  "Multi-Status",    // 207
  "Multi-Status",    // 208
  "IM Used"          // 226
};

// [3xx] Códigos de redirecionamento
static const
char* redirection_codes[] = {
  "Multiple Choice",    // 300
  "Moved Permanently",  // 301
  "Found",              // 302
  "See Other",          // 303
  "Not Modified",       // 304
  "Use Proxy",          // 305
  NULL,                 // 306 (unused)
  "Temporary Redirect", // 307
  "Permanent Redirect", // 308
};

// [4xx] Códigos de erro do cliente
static const
char* client_error_codes[] = {
  "Bad Request",                     // 400
  "Unauthorized",                    // 401
  "Payment Required",                // 402
  "Forbidden",                       // 403
  "Not Found",                       // 404
  "Method Not Allowed",              // 405
  "Not Acceptable",                  // 406
  "Proxy Authentication Required",   // 407
  "Request Timeout",                 // 408
  "Conflict",                        // 409
  "Gone",                            // 410
  "Length Required",                 // 411
  "Precondition Failed",             // 412
  "Payload Too Large",               // 413
  "URI Too Long",                    // 414
  "Unsupported Media Type",          // 415
  "Requested Range Not Satisfiable", // 416
  "Expectation Failed",              // 417
  "I'm a teapot",                    // 418
  NULL,
  NULL,
  "Misdirected Request",             // 421
  "Unprocessable Entity",            // 422
  "Locked",                          // 423
  "Failed Dependency",               // 424
  "Too Early",                       // 425
  "Upgrade Required",                // 426
  NULL,
  "Precondition Required",           // 428
  "Too Many Requests",               // 429
  NULL,
  "Request Header Fields Too Large", // 431
  "Unavailable For Legal Reasons",   // 451
};

// [5xx] Códigos de erro do servidor
static const
char* server_error_codes[] = {
  "Internal Server Error",           // 500
  "Not Implemented",                 // 501
  "Bad Gateway",                     // 502
  "Service Unavailable",             // 503
  "Gateway Timeout",                 // 504
  "HTTP Version Not Supported",      // 505
  "Variant Also Negotiates",         // 506
  "Insufficient Storage",            // 507
  "Loop Detected",                   // 508
  NULL,
  NULL,
  "Not Extended",                    // 510
  "Network Authentication Required"  // 511
}