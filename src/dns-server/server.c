#include "server.h"
#include "args.h"
#include "client.h"
#include "dns_parse.h"
#include "log.h"

#include <netinet/in.h>
#include <uv.h>
#include <uv/unix.h>

uv_loop_t *loop = 0;

uv_udp_t *srv_sock = 0;
uv_udp_t *cli_sock = 0;

static struct sockaddr remote_addr;

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size,
                         uv_buf_t *buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

void on_send(uv_udp_send_t *handle, int status) {
  free(handle);
  if (status) {
    printf("uv_udp_send_cb error: %s\n", uv_strerror(status));
  }
}

static void on_cli_read(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                        const struct sockaddr *addr, unsigned flags) {
  if (nread < 0) {
    printf("recv error: %s\n", uv_err_name(nread));
    uv_close((uv_handle_t *)handle, NULL);
    free(buf->base);
    return;
  }

  if (nread > 0) {
    char sender[17] = {0};
    uint16_t port = ntohs(*(uint16_t *)addr->sa_data);
    uv_ip4_name((const struct sockaddr_in *)addr, sender, 16);
    log_info("recv server response from %s:%d", sender, port);

    // find corresponding query
    uint16_t dns_id = get_dns_id(buf->base);
    int u_id = -1;
    for (size_t i = 0; i < upool->pool_size; i++) {
      if (dns_id == upool->pool[i]->dns_id && upool->pool[i]->valid) {
        u_id = i;
        break;
      }
    }
    if (u_id == -1) {
      log_warn("unrecognized dns id in response");
      free(buf->base);
      return;
    }
    udp_context_t *c = upool->pool[u_id];
    set_dns_id(buf->base, qpool->pool[c->query_id].dns_id);

    // send to client
    uv_udp_send_t *send_req = malloc(sizeof(uv_udp_send_t));
    uv_buf_t send_buf = uv_buf_init(buf->base, nread);
    uv_udp_send(send_req, srv_sock, &send_buf, 1,
                &qpool->pool[c->query_id].addr, on_send);
    upool_finish(upool, u_id);

    free(buf->base);
  }
}

static void on_srv_read(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                        const struct sockaddr *addr, unsigned flags) {
  uv_udp_send_t *req;
  uv_buf_t sndbuf;
  char ipaddr[17] = {0};
  uv_ip4_name(&addr, ipaddr, sizeof(ipaddr));
  if (nread <= 0) {
    printf("[ERROR] Detected %s trans error or null trans, len :%zd !\n",
           ipaddr, nread);
    return;
  }
  printf("[INFO] receive message from %s\n", ipaddr, buf->base);
  printf("%zd\n", nread);
  for (int i = 0; i < nread; i += 4) {
    printf("0x%02x ", ((int *)buf->base)[i / 4]);
  }
  fflush(stdout);
  return;
}

void loop_init() { loop = uv_default_loop(); }

void socket_init(parameter_t *para) {
  srv_sock = malloc(sizeof(uv_udp_t));
  cli_sock = malloc(sizeof(uv_udp_t));

  struct sockaddr_in srv_addr;
  struct sockaddr_in cli_addr;
  uv_ip4_addr("0.0.0.0", 53, &srv_addr);
  uv_ip4_addr("0.0.0.0", para->client_port, &cli_addr);
  uv_ip4_addr(para->server_addr, 53, (struct sockaddr_in *)&remote_addr);

  uv_udp_init(loop, srv_sock);
  uv_udp_bind(srv_sock, (const struct sockaddr *)&srv_addr, UV_UDP_REUSEADDR);
  uv_udp_recv_start(srv_sock, alloc_buffer, on_srv_read);

  uv_udp_init(loop, cli_sock);
  uv_udp_bind(cli_sock, (const struct sockaddr *)&cli_addr, UV_UDP_REUSEADDR);
  uv_udp_recv_start(cli_sock, alloc_buffer, on_cli_read);
}

int server_run() { return uv_run(loop, UV_RUN_DEFAULT); }
