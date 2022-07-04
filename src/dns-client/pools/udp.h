#ifndef UDP_POOL_H
#define UDP_POOL_H

#define UDP_CONTEXT_BUF_SIZE 512

#include "query.h"
#include "queue.h"
#include <stdlib.h>

typedef struct {
  int query_id;
  int dns_id;
  char send_buf[UDP_CONTEXT_BUF_SIZE];
  size_t send_len;
  char valid;
  void *timer;
} udp_context_t;

typedef struct {
  udp_context_t **pool;
  size_t pool_size;
  struct sc_queue_int queue;
  uint16_t next_dns_id;
} udp_pool_t;

udp_pool_t *upool_init(size_t size);

int upool_full(udp_pool_t *pool);

int upool_add(udp_pool_t *pool, int q_id, query_t *q);

void upool_finish(udp_pool_t *pool, int u_id);

void upool_free(udp_pool_t *pool);

#endif
