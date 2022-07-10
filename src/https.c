#include "https.h"
#include "args.h"
#include "client.h"
#include "dns-client/dns_parse.h"
#include "dns-server/server.h"
#include "dns-server/server_cache.h"
#include "log.h"
#include <ctype.h>

static CURLM *curl_handle = 0;
static uv_timer_t timeout;
static parameter_t *para = 0;

typedef struct curl_context_s {
  uv_poll_t poll_handle;
  curl_socket_t sockfd;
} curl_context_t;

static curl_context_t *create_curl_context(curl_socket_t sockfd) {
  curl_context_t *context;
  context = (curl_context_t *)malloc(sizeof(*context));
  context->sockfd = sockfd;
  uv_poll_init_socket(loop, &context->poll_handle, sockfd);
  context->poll_handle.data = context;
  return context;
}

static void curl_close_cb(uv_handle_t *handle) {
  curl_context_t *context = (curl_context_t *)handle->data;
  free(context);
}

static void destroy_curl_context(curl_context_t *context) {
  uv_close((uv_handle_t *)&context->poll_handle, curl_close_cb);
}
