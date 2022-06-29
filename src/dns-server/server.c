#include "server.h"
#include "log.h"

#include <netinet/in.h>
#include <uv.h>
#include <uv/unix.h>

uv_loop_t *loop = 0;

uv_udp_t *srv_sock = 0;
static struct sockaddr remote_addr;

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size,
                         uv_buf_t *buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
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

void socket_init() {
  srv_sock = malloc(sizeof(uv_udp_t));

  struct sockaddr_in srv_addr;
  uv_ip4_addr("0.0.0.0", 53, &srv_addr);

  uv_udp_init(loop, srv_sock);
  uv_udp_bind(srv_sock, (const struct sockaddr *)&srv_addr, UV_UDP_REUSEADDR);
  uv_udp_recv_start(srv_sock, alloc_buffer, on_srv_read);
}

int server_run() { return uv_run(loop, UV_RUN_DEFAULT); }
