#include <stdlib.h>
#include <stdio.h>
#include "hmap.h"
#include "util.h"

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