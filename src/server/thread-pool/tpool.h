#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H
#include <pthread.h>

#define THREAD_POOL_MAX_SIZE 16

typedef struct {
  pthread_t threads[THREAD_POOL_MAX_SIZE];
  size_t counter;
} thread_pool_t;

int thread_pool_create(thread_pool_t* tpool);

#endif __THREAD_POOL_H