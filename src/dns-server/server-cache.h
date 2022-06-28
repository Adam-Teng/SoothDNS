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

#include "database/cache.h"
#include "database/tree.h"
#include "utils/args.h"

extern tree_t *db_tree;
extern cache_t *db_cache;

void server_cache_init(parameter_t *para);

void server_cache_deinit();

#endif
