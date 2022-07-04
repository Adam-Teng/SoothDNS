#ifndef DB_CACHE_H
#define DB_CACHE_H

#include "type.h"
#define DB_CACHE_INDEX_WIDTH 0x10
#define DB_CACHE_ENTRY_NUM (0x1 << DB_CACHE_INDEX_WIDTH)
#define DB_CACHE_WAY_NUM 0x2

typedef struct {
  db_name_t *key;
  db_record_t *rec;
} cache_entry_t;

typedef struct {
  char lru[DB_CACHE_ENTRY_NUM];
  cache_entry_t entries[DB_CACHE_WAY_NUM][DB_CACHE_ENTRY_NUM];
} cache_t;

cache_t *db_cache_init();

void db_cache_insert(cache_t *c, db_name_t *key, db_record_t *val);

db_record_t *db_cache_lookup(cache_t *c, db_name_t *key);

void db_cache_free(cache_t *c);

uint16_t hash_name(db_name_t *name);

#endif
