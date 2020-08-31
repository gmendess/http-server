#include <stdlib.h>
#include <stdio.h>
#include "hmap.h"
#include "util.h"

/*
  Veja: http://www.cse.yorku.ca/~oz/hash.html
*/
static ussize_t djb2_hash(char *key) {
  ussize_t hash = 5381;
  int c = 0;

  while((c = *key++))
    hash = ((hash << 5) + hash) + c;

  return hash;
}

void hmap_init(hmap_t* hmap, size_t capacity) {
  if(capacity == 0)
    capacity = HMAP_DEFAULT_CAPACITY;

  hmap->capacity = capacity;
  hmap->counter  = 0;
  hmap->entries = must_calloc(capacity, sizeof(hmap_entry_t));
}

void hmap_destroy(hmap_t* hmap) {
  hmap->capacity = 0;
  hmap->counter  = 0;
  free(hmap->entries);
  hmap->entries  = NULL;
}

int hmap_add(hmap_t* hmap, char* key, void* value) {
  ussize_t index = djb2_hash(key) % hmap->capacity;

  if(hmap->counter == hmap->capacity)
    return -1;

  hmap->entries[index].key = key;
  hmap->entries[index].value = value;
  hmap->counter++;

  return index;
}