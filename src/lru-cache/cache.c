#include "cache.h"

#include <stdlib.h>
#include <string.h>

uint8_t hash_name(db_name_t *name) {
  if (!name)
    return 0;
  char *s = name->label.label;
  uint8_t res = 0;
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
      destroy_name(c->entries[i][j].key);
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

void db_cache_insert(cache_t *c, db_name_t *key, db_record_t *val) {
  uint16_t idx = hash_name(key);
  char sel = c->lru[idx];
  c->entries[sel][idx].key = dup_name(key);
  c->entries[sel][idx].rec = val;
  c->lru[idx] = sel == 0 ? 1 : 0;
}

int compare_name(db_name_t *a, db_name_t *b) {
  if (a == 0 && b == 0)
    return 0;
  if (a == 0 || b == 0)
    return -1;
  if (strcmp(a->label.label, b->label.label) == 0) {
    return compare_name(a->next, b->next);
  }
  return -1;
}

db_record_t *db_cache_lookup(cache_t *c, db_name_t *key) {
  uint16_t idx = hash_name(key);
  for (size_t i = 0; i < DB_CACHE_WAY_NUM; i++) {
    if (compare_name(key, c->entries[i][idx].key) == 0) {
      c->lru[idx] = i == 0 ? 1 : 0;
      return c->entries[i][idx].rec;
    }
  }
  return 0;
}
