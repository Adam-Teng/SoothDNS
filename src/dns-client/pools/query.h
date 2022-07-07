#ifndef QUERY_H
#define QUERY_H

#include "../../database/type.h"
#include "../dns_parse.h"
#include "queue.h"
#include <sys/socket.h>

typedef struct {
  struct sockaddr addr;
  char *msg;
  size_t msg_len;
  uint16_t dns_id;
  db_name_t *dns_dn_name;
} query_t;

typedef struct {
  query_t *pool;
  size_t pool_size;
  struct sc_queue_int queue;
} qpool_t;

qpool_t *qpool_init(size_t pool_size);

int qpool_full(qpool_t *pool);

int qpool_insert(qpool_t *pool, struct sockaddr addr, char *msg, size_t msg_len,
                 db_name_t *dn_name);

void qpool_remove(qpool_t *pool, size_t idx);

void qpool_free(qpool_t *pool);

#endif
