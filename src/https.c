#include "https.h"
#include "args.h"
#include "client.h"
#include "dns-server/server.h"
#include "dns-server/server_cache.h"
#include "dns_parse.h"
#include "log.h"
#include <ctype.h>

static CURLM *curl_handle = 0;
static uv_timer_t timeout;

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

static size_t handle_data(char *buf, size_t size, size_t nmemb,
                          void *userdata) {
  conn_context_t *context = (conn_context_t *)userdata;
  size_t realsize = size * nmemb;

  memcpy(&context->read_buf[context->nread], buf, realsize);
  context->nread += realsize;

  return realsize;
}

void on_conn_timeout(uv_timer_t *timer) {
  conn_context_t *conn = timer->data;
  log_warn("doh conn %d timeout, delete conn", conn->conn_id);
  uv_timer_stop(timer);
  curl_multi_remove_handle(curl_handle, conn->easy_handle);
  qpool_remove(qpool, conn->query_id);
  cpool_finish(cpool, conn->conn_id);
  free(timer);
}

void add_doh_connection(struct sockaddr addr, char *req_data, size_t req_len,
                        db_name_t *name) {
  // add to query pool
  if (qpool_full(qpool)) {
    log_warn("ignore dns request due to full query pool.");
    return;
  }
  if (cpool_full(cpool)) {
    log_warn("ignore dns request due to full doh conn pool.");
    return;
  }
  int q_id = qpool_insert(qpool, addr, req_data, req_len, name);
  // get curl handle
  conn_context_t *conn = cpool_add_conn(cpool, q_id, qpool->pool + q_id);

  CURL *easy_handle = conn->easy_handle;

  // construct the post request
  // prepare headers
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/dns-message");
  headers = curl_slist_append(headers, "Accept: application/dns-message");
  char s[1024];
  snprintf(s, 1024, "Content-Length: %ld", conn->send_len);
  headers = curl_slist_append(headers, s);
  // prepare body
  curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, conn->send_buf);
  curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDSIZE, conn->send_len);
  curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);

  // set up write handler
  curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, handle_data);

  // verbose
  curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, true);

  // add timeout
  uv_timer_t *timer = malloc(sizeof(uv_timer_t));
  uv_timer_init(loop, timer);
  timer->data = conn;
  uv_timer_start(timer, on_conn_timeout, HTTPS_TIMEOUT, HTTPS_TIMEOUT);
  conn->timeout_timer = timer;

  curl_multi_add_handle(curl_handle, easy_handle);
}

static db_name_t *db_name_from_dns_name(dn_name_t *dns_name, char *raw) {
  db_name_t *name = 0;
  for (size_t i = 0; i < dns_name->len; i++) {
    db_name_t *u = malloc(sizeof(db_name_t));
    u->label.len = dns_name->labels[i].len;
    u->label.label = malloc(sizeof(char) * (u->label.len + 1));
    memcpy(u->label.label, raw + dns_name->labels[i].offset, u->label.len);
    // convert to lower case
    for (size_t j = 0; j < u->label.len; j++) {
      u->label.label[j] = tolower(u->label.label[j]);
    }
    u->label.label[u->label.len] = '\0';
    u->next = name;
    name = u;
  }

  return name;
}

static void check_multi_info(void) {
  char *done_url;
  CURLMsg *message;
  int pending;
  CURL *easy_handle;
  conn_context_t *context;
  int q_id;
  query_t *q;
  int c_id;

  while ((message = curl_multi_info_read(curl_handle, &pending))) {
    switch (message->msg) {
    case CURLMSG_DONE:
      easy_handle = message->easy_handle;

      curl_easy_getinfo(easy_handle, CURLINFO_EFFECTIVE_URL, &done_url);
      curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &context);
      q_id = context->query_id;
      q = qpool->pool + q_id;
      c_id = context->conn_id;

      curl_multi_remove_handle(curl_handle, easy_handle);
      if (context->nread) {
        log_info("receive response from doh server, length %ld",
                 context->nread);
        // change dns message id
        set_dns_id(context->read_buf, q->dns_id);
        uv_udp_send_t *send_req = malloc(sizeof(uv_udp_send_t));
        uv_buf_t send_buf = uv_buf_init(context->read_buf, context->nread);
        uv_udp_send(send_req, srv_sock, &send_buf, 1,
                    &qpool->pool[context->query_id].addr, on_send);

        // parse the message
        dns_msg_t msg;
        int ret_code = parse_dns_msg(context->read_buf, &msg);
        memcpy(msg.raw, context->read_buf, msg.msg_len);
        if (ret_code != DNS_MSG_PARSE_OKAY) {
          log_error("fail to parse dns message");
        } else {
          print_dns_msg(&msg);

          for (int i = 0; i < msg.header.an_cnt; i++) {
            dns_msg_rr_t *rr = msg.answer + i;
            if (rr->type == 1) {
              db_name_t *name = db_name_from_dns_name(&rr->name, msg.raw);
              db_ip_t ip = ntohl(*(uint32_t *)(rr->data_offset + msg.raw));
              uint32_t ttl = rr->ttl;
              //                    uint32_t ttl = 120;

              lc_insert(db_lru_cache, name, ip, ttl);
            }
          }

          for (int i = 0; i < msg.header.ns_cnt; i++) {
            dns_msg_rr_t *rr = msg.authority + i;
            if (rr->type == 1) {
              db_name_t *name = db_name_from_dns_name(&rr->name, msg.raw);
              db_ip_t ip = ntohl(*(uint32_t *)(rr->data_offset + msg.raw));
              uint32_t ttl = rr->ttl;

              lc_insert(db_lru_cache, name, ip, ttl);
            }
          }
        }
      }
      uv_timer_t *timer = context->timeout_timer;
      uv_timer_stop(timer);
      free(timer);
      qpool_remove(qpool, q_id);
      cpool_finish(cpool, c_id);
      break;

    default:
      log_info("CURLMSG default");
      break;
    }
  }
}

static void curl_perform(uv_poll_t *req, int status, int events) {
  int running_handles;
  int flags = 0;
  curl_context_t *context;

  if (events & UV_READABLE)
    flags |= CURL_CSELECT_IN;
  if (events & UV_WRITABLE)
    flags |= CURL_CSELECT_OUT;

  context = (curl_context_t *)req->data;

  curl_multi_socket_action(curl_handle, context->sockfd, flags,
                           &running_handles);

  check_multi_info();
}

static void on_timeout(uv_timer_t *req) {
  int running_handles;
  curl_multi_socket_action(curl_handle, CURL_SOCKET_TIMEOUT, 0,
                           &running_handles);
  check_multi_info();
}

static int start_timeout(CURLM *multi, long timeout_ms, void *userp) {
  if (timeout_ms < 0) {
    uv_timer_stop(&timeout);
  } else {
    if (timeout_ms == 0)
      timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it
                   in a bit */
    uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
  }
  return 0;
}

static int handle_socket(CURL *easy, curl_socket_t s, int action, void *userp,
                         void *socketp) {
  curl_context_t *curl_context;
  int events = 0;

  switch (action) {
  case CURL_POLL_IN:
  case CURL_POLL_OUT:
  case CURL_POLL_INOUT:
    curl_context = socketp ? (curl_context_t *)socketp : create_curl_context(s);

    curl_multi_assign(curl_handle, s, (void *)curl_context);

    if (action != CURL_POLL_IN)
      events |= UV_WRITABLE;
    if (action != CURL_POLL_OUT)
      events |= UV_READABLE;

    uv_poll_start(&curl_context->poll_handle, events, curl_perform);
    break;
  case CURL_POLL_REMOVE:
    if (socketp) {
      uv_poll_stop(&((curl_context_t *)socketp)->poll_handle);
      destroy_curl_context((curl_context_t *)socketp);
      curl_multi_assign(curl_handle, s, NULL);
    }
    break;
  default:
    abort();
  }

  return 0;
}

void curl_init() {
  if (curl_global_init(CURL_GLOBAL_ALL)) {
    log_error("could not init curl");
  }

  uv_timer_init(loop, &timeout);

  curl_handle = curl_multi_init();
  curl_multi_setopt(curl_handle, CURLMOPT_SOCKETFUNCTION, handle_socket);
  curl_multi_setopt(curl_handle, CURLMOPT_TIMERFUNCTION, start_timeout);
}

void curl_deinit() { curl_multi_cleanup(curl_handle); }

void https_init() { curl_init(); }

void https_deinit() { curl_deinit(); }
