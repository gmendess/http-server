#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void hmap_init(hmap_t* hmap, size_t buckets) {
  if(buckets == 0)
    buckets = HMAP_DEFAULT_BUCKETS;

  hmap->buckets = buckets;
  hmap->counter = 0;
  hmap->entries = must_calloc(buckets, sizeof(hmap_entry_t*));
}

static void free_entry_list(hmap_entry_t* entry) {
  hmap_entry_t* prev = NULL;
  while(entry) {
    prev  = entry;
    entry = entry->next;
    free(prev);
  }
}

void hmap_destroy(hmap_t* hmap) {
  for(int i = 0; i < hmap->buckets; i++)
    free_entry_list(hmap->entries[i]);
  free(hmap->entries);
  hmap->buckets  = 0;
  hmap->counter  = 0;
  hmap->entries  = NULL;
}

int hmap_set(hmap_t* hmap, char* key, void* value) {
  ussize_t index = djb2_hash(key) % hmap->buckets;

  hmap_entry_t** entry = &hmap->entries[index];

  // bucket vazio, aloco memória para ele
  if(*entry == NULL)
    *entry  = must_calloc(1, sizeof(hmap_entry_t));
  else if( (*entry)->occupied ) {
    do {
      // se a chave de entry for igual a passada via parâmetro será
      // necessário alterar o valor de uma chave que já existe, encerro
      // o loop sem precisar alocar memória
      if(strcmp( (*entry)->key, key) == 0)
        goto set;

      // vá para a próxima entry da lista
      entry = &(*entry)->next;
    }while(*entry != NULL);

    // cheguei ao final da lista, aloco memória para a nova entry que 
    // será configurada logo abaixo
    *entry = must_calloc(1, sizeof(hmap_entry_t));
  }

  // configurando os valores da nova entry
set:
  (*entry)->key      = key;
  (*entry)->value    = value;
  (*entry)->occupied = 1;

  hmap->counter++;

  return 0;
}

int hmap_get(hmap_t* hmap, char* key, void** output) {
  if(hmap->counter == 0)
    return HMAP_NOT_FOUND;

  ussize_t index = djb2_hash(key) % hmap->buckets;
  hmap_entry_t* entry = hmap->entries[index];

  // bucket ainda não alocado ou não ocupado
  if(!entry || !entry->occupied)
    return HMAP_NOT_FOUND;

  // procuro a entry que tenha a chave solicitada 
  while(strcmp(entry->key, key) != 0) {
    entry = entry->next;

    // se entry for NULL, cheguei ao final da lista sem encontrar
    // a entry que tenha a chave desejada
    if(entry == NULL)
      return HMAP_NOT_FOUND;
  }

  *output = entry->value;

  return 0;
}

void hmap_iterate(hmap_t* hmap, it_function it, void* it_opt_args) {
  hmap_entry_t* entry = NULL;

  for(int i = 0; i < hmap->buckets; i++) {
    entry = hmap->entries[i];
    if(!entry || !entry->occupied)
      continue;

    while(entry) {
      it(entry->key, entry->value, it_opt_args);
      entry = entry->next;
    }

  } // end for
}