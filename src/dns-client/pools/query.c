#include "query.h"
#include <string.h>

qpool_t *qpool_init(size_t pool_size) {
  qpool_t *pool = malloc(sizeof(qpool_t));
  pool->pool_size = pool_size;
  pool->pool = malloc(sizeof(query_t) * pool_size);
  sc_queue_init(&pool->queue);
  for (size_t i = 0; i < pool_size; i++) {
    sc_queue_add_last(&pool->queue, i);
  }

  return pool;
}

int qpool_full(qpool_t *pool) { return sc_queue_empty(&pool->queue); }

int qpool_insert(qpool_t *pool, struct sockaddr addr, char *msg, size_t msg_len,
                 db_name_t *dn_name) {
  assert(!qpool_full(pool));
  sc_queue_del_first(&pool->queue);
  size_t idx = pool->queue.first;
  pool->pool[idx].addr = addr;
  pool->pool[idx].msg = malloc(msg_len);
  memcpy(pool->pool[idx].msg, msg, msg_len);
  pool->pool[idx].msg_len = msg_len;
  pool->pool[idx].dns_id = get_dns_id(msg);
  pool->pool[idx].dns_dn_name = dn_name;
  return idx;
}

void qpool_remove(qpool_t *pool, size_t idx) {
  if (pool->pool[idx].msg) {
    free(pool->pool[idx].msg);
  }
  if (pool->pool[idx].dns_dn_name) {
    // destroy name when removing the handle
    destroy_name(pool->pool[idx].dns_dn_name);
  }
  sc_queue_add_last(&pool->queue, idx);
}

void qpool_free(qpool_t *pool) {
  sc_queue_term(&pool->queue);
  free(pool->pool);
  free(pool);
}
