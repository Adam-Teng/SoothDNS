#include "server.h"
#include "utils/log.h"

#include <uv.h>

uv_loop_t *loop = 0;

void loop_init() { loop = uv_default_loop(); }
