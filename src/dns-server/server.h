#ifndef SERVER_H
#define SERVER_H

#include "args.h"
#include <stdlib.h>
#include <uv.h>

#define UDP_TIMEOUT 8000

extern uv_loop_t *loop;
extern uv_udp_t *srv_sock, *cli_sock;

void loop_init();
void socket_init(parameter_t *para);

int server_run();

#endif
