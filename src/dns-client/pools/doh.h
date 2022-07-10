#ifndef DOH_H
#define DOH_H

#include "query.h"
#include "queue.h"
#include <curl/curl.h>

#define CONN_CONTEXT_BUF_SIZE 512

typedef struct {
  CURL *easy_handle;
  int query_id; // associated query id
  char read_buf[CONN_CONTEXT_BUF_SIZE];
  char send_buf[CONN_CONTEXT_BUF_SIZE];
  size_t send_len;
  size_t nread;
  uint16_t dns_id;
  int conn_id;
  void *timeout_timer;
  int retry_count;
} conn_context_t;

typedef struct {
  conn_context_t **pool;
  size_t pool_size;
  struct sc_queue_int queue;
  uint16_t next_dns_id;
} conn_pool_t;

conn_pool_t *cpool_init(char *doh_server);

int cpool_full(conn_pool_t *pool);

conn_context_t *cpool_add_conn(conn_pool_t *pool, int q_id, query_t *q);

void cpool_finish(conn_pool_t *pool, int c_id);

void cpool_free(conn_pool_t *pool);

#endif
