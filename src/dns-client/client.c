#include "client.h"
#include "query.h"
#include "udp.h"

qpool_t *qpool = 0;
udp_pool_t *upool = 0;

void pools_init(parameter_t *para) {
  qpool = qpool_init(para->max_query);
  upool = upool_init(para->max_udp_req);
}

void pools_deinit() {
  qpool_free(qpool);
  upool_free(upool);
}
