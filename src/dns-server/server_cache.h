<<<<<<< HEAD
/*
 * ============================================================================
 *
 *       Filename:  server-cache.h
 *
 *    Description:  init the database and cache
 *
 *        Created:  06/28/2022
 *
 *         Author:  yhteng
 *
 * ============================================================================
 */

#ifndef DNS_SERVER_CACHE_H
#define DNS_SERVER_CACHE_H

#include "args.h"
#include "cache.h"
=======
#ifndef DNS_SERVER_CACHE_H
#define DNS_SERVER_CACHE_H

#include "../lru-cache/cache.h"
#include "../lru-cache/lru.h"
#include "args.h"
>>>>>>> dev
#include "tree.h"

extern tree_t *db_tree;
extern cache_t *db_cache;
<<<<<<< HEAD
=======
extern trie db_lru_cache;
>>>>>>> dev

void server_cache_init(parameter_t *para);

void server_cache_deinit();

#endif
