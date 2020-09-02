#ifndef __HASH_MAP_H
#define __HASH_MAP_H

#include <stdlib.h>

#define HMAP_DEFAULT_BUCKETS 256
#define HMAP_NOT_FOUND -1
#define HMAP_FULL      -2

typedef unsigned long ussize_t;

typedef struct hmap_entry {
  char* key;
  void* value;
  int   occupied;
  struct hmap_entry* next;
} hmap_entry_t;

typedef struct {
  size_t buckets;
  size_t counter;
  hmap_entry_t** entries;
} hmap_t;

typedef void(*it_function)(const char* key, void* value, void* opt_args);

void hmap_init(hmap_t* hmap, size_t buckets);
void hmap_destroy(hmap_t* hmap);

int hmap_set(hmap_t* hmap, char* key, void* value);
int hmap_get(hmap_t* hmap, char* key, void** output);

void hmap_iterate(hmap_t* hmap, it_function it, void* it_opt_args);

#endif // __HASH_MAP_H