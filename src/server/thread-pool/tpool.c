#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "tpool.h"
#include "../../util/util.h"

int thread_pool_create(thread_pool_t* tpool) {
  int err = 0; // controle de erros
  int total_errors = 0; // total de erros ocorridos

  // zero threads ativas
  tpool->counter = 0;

  // cria atributo das threads, no caso, cada thread criada será "detached", para ter seus
  // recursos liberados logo após o término.
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  // tenta criar a quantidade de threads especificada por THREAD_POOL_MAX_SIZE 
  for(int i = 0; i < THREAD_POOL_MAX_SIZE; i++) {
    // cria a thread
    err = pthread_create(&tpool->threads[tpool->counter], &attr, NULL/*routine*/, NULL);
    
    // verifica se ocorreu erro na criação
    if(err != 0) {
      perror("pthread_create");
      total_errors++;
    }
    else
      tpool->counter++; // se não ocorreu nenhum erro, incrementa o contador de threads ativas
  }

  // retorna número de erros
  return total_errors;
}

int conn_queue_init(conn_queue_t* queue, size_t capacity) {
  if(capacity == 0)
    return -1;

  int err = 0; // controle de erros

  err |= pthread_mutex_init(&queue->mu, NULL);  
  err |= sem_init(&queue->semaphore, 0, 0);

  if(err) {
    perror("conn_queue_init");
    exit(EXIT_FAILURE); // erro fatal, não foi possível inicializar ou a mutex ou o semáforo
  }

  queue->connections = must_calloc(capacity, sizeof(int));
 
  queue->capacity = capacity;
  queue->head     = 0;
  queue->tail     = 0;
  queue->counter  = 0;

  return 0;
}

int conn_enqueue(conn_queue_t* queue, int conn_fd) {
  // adquire a mutex para fazer alterações na fila
  pthread_mutex_lock(&queue->mu);
  
  // fila cheia
  if(queue->counter == queue->capacity) {
    pthread_mutex_unlock(&queue->mu);
    return -1;
  }

  queue->connections[queue->tail] = conn_fd; // insere a conexão no vetor de conexões
  queue->tail = (queue->tail + 1) % queue->capacity; // cria efeito de circularidade na fila
  queue->counter++; // incrementa a quantidade de conexões na fila

  // libera a mutex
  pthread_mutex_unlock(&queue->mu);

  // incrementa contador do semáforo, indicando que uma conexão foi adicionada na fila
  sem_post(&queue->semaphore);
  return 0;
}

int conn_dequeue(conn_queue_t* queue) {
  // verifica se há alguma conexão a ser gerenciada, se não, espera por uma
  sem_wait(&queue->semaphore);

  // adquire mutex para fazer alterações e leitura na fila
  pthread_mutex_lock(&queue->mu);

  int ret_val = queue->connections[queue->head]; // adquire a conexão no inicio da fila
  queue->head = (queue->head + 1) % queue->capacity; // move o inicio da fila
  queue->counter--; // decrementa a quantidade de conexões

  // libera a mutex
  pthread_mutex_unlock(&queue->mu);

  // retorna o file descriptor da conexão
  return ret_val;
}