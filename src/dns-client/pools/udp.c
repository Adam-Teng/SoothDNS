#include "udp.h"
#include <time.h>

udp_pool_t *upool_init(size_t size) {
  udp_pool_t *pool = malloc(sizeof(udp_pool_t));
  pool->pool_size = size;
  pool->next_dns_id = time(0);
  // init pool
  pool->pool = malloc(sizeof(void *) * size);
  for (size_t i = 0; i < size; i++) {
    pool->pool[i] = malloc(sizeof(udp_context_t));
    pool->pool[i]->valid = 0;
  }
  // init idx queue
  sc_queue_init(&pool->queue);
  for (size_t i = 0; i < size; i++) {
    sc_queue_add_last(&pool->queue, i);
  }

  return pool;
}

int upool_full(udp_pool_t *pool) { return sc_queue_empty(&pool->queue); }

int upool_add(udp_pool_t *pool, int q_id, query_t *q) {
  assert(!upool_full(pool));
  sc_queue_del_first(&pool->queue);
  size_t u_id = pool->queue.first;
  pool->pool[u_id]->valid = 1;
  pool->pool[u_id]->query_id = q_id;
  pool->pool[u_id]->dns_id = pool->next_dns_id++;
  pool->pool[u_id]->send_len = q->msg_len;
  memcpy(pool->pool[u_id]->send_buf, q->msg, q->msg_len);
  set_dns_id(pool->pool[u_id]->send_buf, pool->pool[u_id]->dns_id);

  return u_id;
}

void upool_finish(udp_pool_t *pool, int u_id) {
  int test = 0;
  for (size_t i = 0; i < sc_queue_size(&pool->queue); i++) {
    int id = (int)u_id;
    test = id == sc_queue_at(&pool->queue, i);
  }
  assert(test);
  sc_queue_add_last(&pool->queue, u_id);
  pool->pool[u_id]->valid = 0;
}

void upool_free(udp_pool_t *pool) {
  sc_queue_term(&pool->queue);
  for (size_t i = 0; i < pool->pool_size; i++) {
    free(pool->pool[i]);
  }
  free(pool->pool);
  free(pool);
}
