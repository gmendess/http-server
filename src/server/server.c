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

#ifndef __USE_XOPEN2K8
#define __USE_XOPEN2K8
#endif

#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "server.h"
#include "../util/util.h"
#include "../errors/errors.h"
#include "../http/request/request.h"
#include "../http/response/response.h"
#include "thread-pool/tpool.h"

/*
  Percorre a lista encadeada @ai tentando criar um socket e associá-lo à algum endereço dessa lista

  @param server: servidor que terá o socket associado à um endereço de @ai
  @param ai: lista encadeada de endereços(retornada por getaddrinfo) em que um de seus endereços
   será associado ao servidor 
*/
static int bind_server(server_t* server, struct addrinfo* ai) {
  int listener = 0; // file descriptor do socket do servidor

  // ponteiro auxiliar para percorrer a lista
  struct addrinfo* aux = NULL;

  // percorre lista encadeada de endereços até conseguir criar socket e dar bind
  for(aux = ai; aux != NULL; aux = aux->ai_next) {

    // tenta adquirir um socket com as configurações do endereço em 'aux'
    listener = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
    if(listener == -1) {
      perror("bind_server: socket");
      continue;
    }

    // tenta reaproveitar o endereço e serviço caso já esteja em uso 
    int boolean = 1; // true
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &boolean, sizeof(boolean));

    // tenta dar bind no endereço contido em 'aux'
    if(bind(listener, aux->ai_addr, aux->ai_addrlen) == -1) {
      perror("bind_server: bind");
      continue;
    }

    // se chegar até aqui, foi possível adquirir um socket e dar bind, encerra o loop
    break;
  }

  // se aux == NULL, cheguei ao final da lista sem conseguir criar o socket e dar bind
  if(aux == NULL)
    return ERR_BIND;

  get_addr_and_port(aux->ai_addr, &server->port, server->addr, sizeof(server->addr));
  server->listener = listener;

  // não preciso mais de 'ai', libero sua memória
  freeaddrinfo(ai);

  return 0;
}

/*
  Libera os recursos alocados para um server_t, no caso, apenas a lista 
  encadeada de rotas.

  @param server: servidor que terá os recursos desalocados
*/
static void free_server(server_t* server) {
  route_t* aux = server->route;
  route_t* prev = NULL;
  while(aux) {
    prev = aux;
    aux = aux->next;
    free(prev);
  }

  if(server->type == THREADED) {
    conn_queue_destroy(server->queue);
    free(server->queue);
  }

  if(server->listener >= 0)
    close(server->listener);
}

/*
  Função responsável por lidar com a requisição de um cliente

  @param server: informações do servidor
  @param clientfd: file descriptor para se comunicar com o cliente
*/
static void handle_request(server_t* server, int clientfd) {
  int err = 0; // controle de erros

  char buffer[1024] = {0};
  recv(clientfd, buffer, sizeof(buffer), 0);

  request_t req = {0};
  hmap_init(&req.header, 256);

  if( (err = parse_request(buffer, &req)) != 0 )
    http_error("parse_request", err);
  else {
    // response padrão
    response_t resp = {0};
    hmap_init(&resp.header, 256);

    resp.clientfd = clientfd;
    resp.status = OK;
    add_header_field(&resp.header, "Date", gmt_date_now());

    // major version da requisição não é suportada pelo servidor
    if(req.req_line.version.major != 1)
      send_default_505(&resp);

    route_t* route = NULL;
    err = find_route(server, req.req_line.path, req.req_line.method, &route);
    if(err == ERR_ROUTENOTFOUND)
      send_default_404(&resp);
    else if(err == ERR_ROUTEMET)
      send_default_405(&resp);
    else
      route->handler(&resp, &req);

    // libera memória alocada para o header da resposta
    hmap_iterate(&resp.header, free_header_field, NULL);
    hmap_destroy(&resp.header);
  }

  // libera memória alocada para conter informações sobre a requisição
  hmap_iterate(&req.header, free_header_field, NULL);
  free_request(&req);

  // caso o servidor use processos filhos para lidar com cada conexão libera sua 
  // memória e encerra o programa
  if(server->type == FORKED) {
    free_server(server);
    exit(err);
  }
}

/*
  Função estática que monitora a fila de conexões para poder adquirir uma conexão e poder
  gerenciá-la. É usada quando o servidor é configurado para ser um threaded-server

  @param args: espera-se que seja um conn_queue_t* (@args é convertido no corpo da função)
*/
static void* thread_acquire_connection(void* args) {
  server_t* server = args; // converte void* para server_t*

  int conn = 0;
  while(1) {
    conn = conn_dequeue(server->queue);
    if(conn == -1)
      break; // fila foi fechada, não é mais possíve retirar mais nada dela

    handle_request(server, conn);
    close(conn);
  }

  return 0;
}

/*
  Cria um novo servidor http

  @param server: estrutura que conterá informações do servidor
  @param addr: endereço do servidor 
  @param port: serviço (porta)
*/
int new_http_server(server_t* server, const char* addr, const char* port) {
  int err = 0; // controle de erros

  // lista de rotas vazia
  server->route = NULL;

  // inicialmente não instancia nenhum fila de conexões (ver config_threaded_server)
  server->queue = NULL;

  struct addrinfo hints = {0};
  hints.ai_family   = AF_UNSPEC;   // IPv4 ou IPv6
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags    = AI_PASSIVE;

  // retorno de getaddrinfo
  struct addrinfo* ai = NULL;

  // adquire lista de endereços do host/serviço
  err = getaddrinfo(addr, port, &hints, &ai);
  if(err != 0)
    return ERR_ADDRLIST;

  // associado o servidor com algum endereço disponível na lista 'ai'
  if( (err = bind_server(server, ai)) != 0)
    return err;

  return 0;
}

/*
  Configura o servidor para ser um threaded server, ou seja, ele usará uma thread para lidar com cada
  conexão. Essas threads fazem parte de uma thread pool.

  @param server: servidor a ser configurado
  @param queue_size: tamanho da fila de conexões; cada conexão aceita será enfileirada e posteriormente
   pega por uma thread que estiver disponível na pool. 
*/
void config_threaded_server(server_t* server, size_t queue_size) {
  server->type = THREADED;
  server->queue = must_calloc(1, sizeof(conn_queue_t));

  // se queue_size for 0, configura para o valor padrão  10
  queue_size = (queue_size > 0) ? queue_size : 10;

  conn_queue_init(server->queue, queue_size);
}

/*
  Inicia o processo de escuta por requisição, programa entra em loop infinito esperando
  por conexões.
  Retorna diferente de 0 em caso de erro

  @param server: estrutura server_t contendo informações do servidor a ser iniciado 
*/
int start_listening(server_t* server) {
  // coloca o socket em estado de listening, escutando por requição de conexão
  if(listen(server->listener, BACKLOG) == -1)
    return ERR_LISTEN;

  // variável para configurar handler de sinais
  struct sigaction sa = {0};
  sa.sa_handler = sigint_handler; // função handler para SIGINT
  sigemptyset(&sa.sa_mask);       // zera a máscara de sinais a serem bloqueados
  sigaction(SIGINT, &sa, NULL);   // configura o handler para SIGINT

  thread_pool_t tpool = {0};
  if(server->type == THREADED) {
    thread_pool_init(&tpool, thread_acquire_connection, (void*) server);
  }
  else {
    // configurando handler para SIGCHLD
    sa.sa_handler = sigchld_handler; // função handler do sinal
    sa.sa_flags   = SA_RESTART;      // reinicia uma syscall que foi interrompida pelo sinal
    sigemptyset(&sa.sa_mask);        // zera a máscara de sinais a serem bloqueados
    sigaction(SIGCHLD, &sa, NULL);   // configura a ação a ser tomada ao receber um SIGCHLD
  }

  // loop infinito para ficar aceitando por requisições
  while(1) {
    struct sockaddr_storage ss;
    unsigned size = sizeof(ss);

    // aceita uma conexão com o cliente
    int clientfd = accept(server->listener, (struct sockaddr*) &ss, &size);
    if(clientfd == -1) {
      if(errno == EINTR)
        break; // SIGINT recebido, encerra o loop
      perror("accept");
      continue;
    }

    if(server->type == THREADED ) {
      if(conn_enqueue(server->queue, clientfd) != 0)
        fprintf(stderr, "fila de conexões cheia! Não foi possível inserir o file descriptor %d.\n", clientfd);
    }
    else {
      // cria um processo filho para lidar com a requisição
      pid_t pid = fork();
      if(pid == 0) {
        // processo filho
        close(server->listener); // fecha o socket do listener, pois o processo filho não precisa dele
        server->listener = -1;   // configura o socket como um inválido, por favor, não usar
        handle_request(server, clientfd); // lida com a requisição
      }
      else if(pid > 0) {
        // processo pai
        close(clientfd); // fecha o socket do cliente, pois o processo pai não precisa dele
        puts("Requisição aceita!");
      }
      else
        perror("fork");
    }

  }

  // verifica se é um threaded-server para sincronizar o término do servidor com o término das threads da pool
  if(server->type == THREADED) {
    /*
      EXPLICAÇÃO: não sei se há forma melhor de fazer isso, mas ok. Basicamente a  fila de conexões tem uma flag chamada `is_open`, essa flag é uma
        variável atômica,  dessa forma, sua leitura e escrita  são garantidas de  serem thread-safe. Quando o  servidor recebe um  SIGINT, o código
        abaixo será executado, marcando a  flag da fila como false, e depois disso incrementando o  contador do semáforo pela quantidade de threads
        existentes na pool. Isso fará com que threads que estejam bloqueadas num sem_wait (ver conn_dequeue) acordem; após acordarem elas verificam
        a flag `is_open`, retornando -1 caso a fila esteja fechada. Depois disso, o servidor espera que todas as threads da pool terminem, ou seja,
        espera que todas as requisições em andamento terminem. Por fim, a memória do servidor é liberada.
    */

    // fecha a fila de conexões para não poder ser mais manipulada
    server->queue->is_open = 0;

    // acorda todas as threads da pool que estiverem bloqueadas em um sem_wait
    for(int x = 0; x < tpool.counter; x++)
      sem_post(&server->queue->semaphore);

    // aguarda a finalização das threads
    for(int x = 0; x < tpool.counter; x++)
      pthread_join(tpool.threads[x], NULL);
  }

  // libera a memória do servidor
  free_server(server);
  return 0;
}

/*
  Adiciona um handler para um determinada rota do servidor
  Retorna diferente de 0 em caso de erro

  @param server: servidor que passará a lidar com a rota especificada em @route
  @param route: nome da rota
  @param handler: função handler que será executada quando um cliente acessar @route 
  @param m_code: código do método HTTP (veja methods.h)
*/
int handle_route(server_t* server,
                 const char* route,
                 route_handler_t handler,
                 http_method_t   method)
{
  if(method < 0 || method > 6)
    return ERR_HTTPMET;
  else if(!route || route[0] == '\0')
    return ERR_ROUTE;
  else if(!server)
    return ERR_SERVER;

  // aloca memória para um route_t
  route_t* new_route = must_calloc(1, sizeof(route_t));

  // configurando route_t
  new_route->path    = route;
  new_route->handler = handler;
  new_route->method  = method;
  new_route->next    = server->route;
  // ligando a nova rota à lista de rotas
  server->route      = new_route;

  return 0;
}

/*
  Procura uma rota de nome @path contida na lista de rotas do servidor
  Retorna diferente de 0 em caso de erro

  @param server: servidor que será executada a busca pela rota
  @param path: nome da rota
  @param method: método HTTP
  @param out: ponteiro onde o handler da rota será retornado
*/
int find_route(server_t* server, const char* path, http_method_t method, route_t** out) {
  int route_found = 0;
  route_t* aux = server->route;
  while(aux != NULL) {
    if(strcmp(aux->path, path) == 0) {
      route_found = 1; // rota encontrada

      // rota encontrada e método HTTP suportado
      if(aux->method == method) {
        *out = aux;
        return 0; // sucesso
      }
    }
    aux = aux->next;
  }

  if(!route_found)
    return ERR_ROUTENOTFOUND;
  else
    return ERR_ROUTEMET;
}