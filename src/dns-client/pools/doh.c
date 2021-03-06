#include "doh.h"
#include <string.h>
#include <time.h>

CURL *setup_handle(conn_context_t *c, char *doh_server) {
  CURL *easy_handle = curl_easy_init();
  curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, c);
  curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, c);

  char url[512];
  snprintf(url, 512, "https://%s/dns-query", doh_server);
  curl_easy_setopt(easy_handle, CURLOPT_URL, url);

  return easy_handle;
}

conn_context_t *make_ctxt(char *doh_server) {
  conn_context_t *c = malloc(sizeof(conn_context_t));
  c->dns_id = 0;
  c->nread = 0;
  c->query_id = 0;
  c->easy_handle = setup_handle(c, doh_server);
  c->timeout_timer = 0;
  c->retry_count = 0;

  return c;
}

conn_pool_t *cpool_init(char *doh_server) {
  conn_pool_t *pool = malloc(sizeof(conn_pool_t));
  pool->pool_size = 32;
  // init queue
  sc_queue_init(&pool->queue);
  for (size_t i = 0; i < 32; i++) {
    sc_queue_add_last(&pool->queue, i);
  }
  // init dns id
  pool->next_dns_id = time(0);
  // setup handles
  pool->pool = malloc(32 * sizeof(void *));
  for (int i = 0; i < 32; i++) {
    pool->pool[i] = make_ctxt(doh_server);
    pool->pool[i]->conn_id = i;
  }

  return pool;
}

int cpool_full(conn_pool_t *pool) { return sc_queue_empty(&pool->queue); }

conn_context_t *cpool_add_conn(conn_pool_t *pool, int q_id, query_t *q) {
  assert(!cpool_full(pool));
  sc_queue_del_first(&pool->queue);
  size_t sel = pool->queue.first;
  conn_context_t *c = pool->pool[sel];
  c->query_id = q_id;
  c->nread = 0;
  c->dns_id = pool->next_dns_id++;
  memcpy(c->send_buf, q->msg, q->msg_len);
  c->send_len = q->msg_len;
  set_dns_id(c->send_buf, c->dns_id);
  c->retry_count = 0;

  return c;
}

void cpool_free(conn_pool_t *pool) {
  for (int i = 0; i < pool->pool_size; i++) {
    curl_easy_cleanup(pool->pool[i]->easy_handle);
    free(pool->pool[i]->easy_handle);
    free(pool->pool[i]);
  }
  free(pool->pool);
  sc_queue_term(&pool->queue);
  free(pool);
}

void cpool_finish(conn_pool_t *pool, int c_id) {
  sc_queue_add_last(&pool->queue, c_id);
}
