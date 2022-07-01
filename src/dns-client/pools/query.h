#ifndef QUERY_H
#define QUERY_H

#include "dns_parse.h"
#include "queue.h"
#include <sys/socket.h>

typedef struct {
  struct sockaddr addr;
  char *msg;
  size_t msg_len;
  uint16_t dns_id;
} query_t;

typedef struct {
  query_t *pool;
  size_t pool_size;
  struct sc_queue_int queue;
} qpool_t;

qpool_t *qpool_init(size_t pool_size);

int qpool_full(qpool_t *pool);

int qpool_insert(qpool_t *pool, struct sockaddr addr, char *msg,
                 size_t msg_len);

void qpool_remove(qpool_t *pool, size_t idx);

void qpool_free(qpool_t *pool);

#endif
