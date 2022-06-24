/*
 * ============================================================================
 *
 *       Filename:  server.h
 *
 *    Description:  header file for server.c
 *
 *        Created:  05/16/22
 *
 *         Author:  yhteng
 *
 * ============================================================================
 */
#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <uv.h>

#include "args.h"
#include "db_cache.h"
#include "db_parse.h"
#include "db_tree.h"

extern uv_loop_t* loop;
extern tree_t* db_tree;
extern cache_t* db_cache;

void server_init(parameter_t* para);

int server_run();

void server_deinit();

#endif
