#include "db_cache.h"
#include "db_type.h"
#include <stdlib.h>
#include <string.h>

uint16_t hash_name(db_name_t *name) {
  if (!name)
    return 0;
  char *str = name->label.label;
  uint16_t res = 0;
  for (size_t i = 0; i < name->label.len; i++) {
    res += res * 31 + name->label.label[i];
  }
  return res ^ hash_name(name->next);
}

cache_t *db_cache_init() {
  cache_t *c = malloc(sizeof(cache_t));
  memset(c, 0, sizeof(cache_t));
  return c;
}

void db_cache_free(cache_t *c) {
  for (size_t i = 0; i < DB_CACHE_WAY_NUM; i++) {
    for (size_t j = 0; j < DB_CACHE_ENTRY_NUM; j++) {
      destroy_name(c->entry[i][j].name);
    }
  }
  free(c);
}

db_name_t *dup_name(db_name_t *name) {
  if (!name)
    return 0;
  db_name_t *ret = malloc(sizeof(db_name_t));
  ret->label.len = name->label.len;
  ret->label.label = strdup(name->label.label);
  ret->next = dup_name(name->next);
  return ret;
}
