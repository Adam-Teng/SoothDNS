#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "trie.h"
#include "type.h"
#include <time.h>

#define LRU_CACHE_SIZE 256

#define now() time(0)

typedef struct lru_cache_node_s {
  db_record_t *record;
  time_t expire_at;
} lru_cache_node_t;

char *compose_trie_key(db_name_t *name);

trie trie_insert_db_dn(trie t, db_name_t *name, void *val);

trie lc_init();

void lc_insert(trie t, db_name_t *name, db_ip_t ip, uint32_t ttl);

lru_cache_node_t *lc_lookup(trie t, db_name_t *name);

lru_cache_node_t *lc_node_init(db_name_t *name, db_ip_t ip, uint32_t ttl);

void lc_node_deinit(lru_cache_node_t *node);

#endif
