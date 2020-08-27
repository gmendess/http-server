#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H
#include <pthread.h>
#include <semaphore.h>

#define THREAD_POOL_MAX_SIZE 16

/*
  Estrutura que representa uma pool de threads. Cada thread dessa pool ficará aguardando até
  que uma conexão esteja disponível para ser gerenciada.
*/
typedef struct {
  pthread_t threads[THREAD_POOL_MAX_SIZE]; // identificador de cada thread
  size_t    counter; // quantidade de threads ativas
} thread_pool_t;

/*
  Fila de conexões disponíveis para serem gerenciadas por uma das threads da pool.

  Esse fila é implementada através de um vetor de inteiros e para isso foi implementado um algoritmo
  para simular uma circularidade, isso fica mais claro nas funções enqueue e dequeue
*/
typedef struct {
  pthread_mutex_t mu; // mutex para evitar race condition nos membros dessa struct
  sem_t  semaphore;   // semáforo para indicar quando há conexões a serem retiradas da fila
  size_t head;        // índice que corresponde ao inicio da fila (não é necessariamente 0)
  size_t tail;        // índice que corresponde ao final da fila, ou seja, onde o próximo elemento será adicionado
  size_t counter;     // número atual de elementos
  size_t capacity;    // capacidade máxima da fila
  int*   connections; // vetor que cada conexão(file descriptor) será enfileirada
} conn_queue_t;

/*
  Inicia uma struct thread_pool_t com seus valores padrões e cria a quantidade de threads
  especificadas por THREAD_POOL_MAX_SIZE

  param tpool: ponteiro para struct thread_pool_t que será inicializada
*/
int thread_pool_create(thread_pool_t* tpool);

/*
  Inicializa os membros de uma struct conn_queue_t

  @param queue: fila à ser inicializada
  @param capacity: capacidade máxima de conexões que podem ser enfileiradas
*/
int conn_queue_init(conn_queue_t* queue, size_t capacity);

/*
  Enfileira uma conexão para ser consumida por uma das threads da pool
*/
int conn_enqueue(conn_queue_t* queue, int conn_fd);

/*
  Retira uma conexão da fila (usado pela threads da pool)

  @param queue: fila em que a conexão será retirada
*/
int conn_dequeue(conn_queue_t* queue);

/*
  Destrói uma conn_queue_t, liberando seus recursos

  @param queue: fila a ser destruída
*/
int conn_queue_destroy(conn_queue_t* queue);

#endif // __THREAD_POOL_H