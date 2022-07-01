#ifndef DNS_CLIENT_H
#define DNS_CLIENT_H

#include "args.h"
#include "log.h"
#include "pools/query.h"
#include "pools/udp.h"

extern qpool_t *qpool;
extern udp_pool_t *upool;

void pools_init(parameter_t *para);

void pools_deinit();

#endif
