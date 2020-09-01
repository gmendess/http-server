#ifndef __HASH_MAP_H
#define __HASH_MAP_H

#include <stdlib.h>

#define HMAP_DEFAULT_CAPACITY 256
#define HMAP_NOT_FOUND -1
#define HMAP_FULL      -2

typedef unsigned long ussize_t;

typedef struct {
  char* key;
  void* value;
  int   occupied;
} hmap_entry_t;

typedef struct {
  size_t capacity;
  size_t counter;
  hmap_entry_t* entries;
} hmap_t;

void hmap_init(hmap_t* hmap, size_t capacity);
void hmap_destroy(hmap_t* hmap);

int hmap_add(hmap_t* hmap, char* key, void* value);
int hmap_get(hmap_t* hmap, char* key, void** output);

#endif // __HASH_MAP_H