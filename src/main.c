#include <stdio.h>
#include <stdlib.h>

#include "dns-client/client.h"
#include "dns-server/server.h"
#include "dns-server/server_cache.h"
#include "https.h"
#include "utils/args.h"
#include "utils/log.h"

static parameter_t *para = 0;

int main(int argc, char *argv[]) {
  /* Init parameters */
  para = parameter_init();

  /* Init ArgParser */
  ArgParser *parser = ap_new();
  if (!parser) {
    exit(1);
  }

  /* Register the program's helptext and version number */
  char *help_info = help();
  ap_set_helptext(parser, help_info);
  ap_set_version(parser, "1.0");

  /* Register program's options */
  ap_str_opt(parser, "server s", "114.114.114.114");
  ap_int_opt(parser, "max_query", 32);
  ap_int_opt(parser, "max_udp_req", 32);
  ap_int_opt(parser, "client_port c", 2345);
  ap_str_opt(parser, "host_path h",
             "/mnt/e/school/network/dns-relay/hosts.txt");
  ap_int_opt(parser, "doh_proxy p", 0);
  ap_str_opt(parser, "doh_server d", "cloudflare-dns.com");

  /* Parse the command line arguments. */
  if (!ap_parse(parser, argc, argv)) {
    exit(1);
  }

  /* Read command line options */
  para->server_addr = ap_str_value(parser, "server");
  para->max_query = ap_int_value(parser, "max_query");
  para->max_udp_req = ap_int_value(parser, "max_udp_req");
  para->client_port = ap_int_value(parser, "client_port");
  para->host_path = ap_str_value(parser, "host_path");
  para->doh_server = ap_str_value(parser, "d");
  para->doh_proxy = ap_int_value(parser, "p");

  log_info("soothDNS started");
  if (para->doh_proxy) {
    log_info("starting as doh proxy, remote server: %s", para->doh_server);
  } else {
    log_info("starting as udp proxy, remote server: %s", para->server_addr);
  }

  /* prints parser's state */
  ap_print(parser);

  /* init server */
  loop_init();
  pools_init(para);
  server_cache_init(para);
  socket_init(para);
  https_init();
  log_info("server is listening at port 53");

  /* run server */
  int ret = server_run();

  pools_deinit();
  server_cache_deinit();
  https_deinit();
  /* Free the parser's memory */
  ap_free(parser);
  return ret;
}
