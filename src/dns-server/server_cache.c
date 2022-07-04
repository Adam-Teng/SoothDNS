/*
 * ============================================================================
 *
 *       Filename:  server-cache.c
 *
 *    Description:  init the database and cache
 *
 *        Created:  06/28/2022
 *
 *         Author:  yhteng
 *
 * ============================================================================
 */

#include "server_cache.h"
#include "cache.h"
#include "io.h"
#include "log.h"
#include "tree.h"
#include "type.h"

tree_t *db_tree = 0;
cache_t *db_cache = 0;
static db_record_t *db_rec = 0;

void server_cache_init(parameter_t *para) {
  int count = 0;
  int db_ret_code = 0;
  db_rec = readfile("/mnt/e/school/network/dns-relay/hosts.txt", &count,
                    &db_ret_code);
  if (db_ret_code != DB_PARSE_RECORD_EOF) {
    log_error("Error parsing hosts file.");
    log_fatal("Fatal error when starting server.");
    exit(1);
  }
  db_tree = tree_build_from_rec(db_rec, count);
  db_cache = db_cache_init();
}

void server_cache_deinit() { free(db_rec); }
