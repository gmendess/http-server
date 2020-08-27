#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "tpool.h"

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