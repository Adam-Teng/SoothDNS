#ifndef DNS_SERVER_CACHE_H
#define DNS_SERVER_CACHE_H

#include "../lru-cache/cache.h"
#include "../lru-cache/lru.h"
#include "args.h"
#include "tree.h"

extern tree_t *db_tree;
extern cache_t *db_cache;
extern trie db_lru_cache;

void server_cache_init(parameter_t *para);

void server_cache_deinit();

#endif
