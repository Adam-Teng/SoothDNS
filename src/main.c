#include <stdio.h>
#include <stdlib.h>

#include "dns-client/client.h"
#include "dns-server/server.h"
#include "dns-server/server_cache.h"
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

  log_info("soothDNS started");

  /* prints parser's state */
  ap_print(parser);

  /* init server */
  loop_init();
  pools_init(para);
  server_cache_init(para);
  socket_init(para);
  log_info("server is listening at port 53");

  /* run server */
  int ret = server_run();

  pools_deinit();
  server_cache_deinit();
  /* Free the parser's memory */
  ap_free(parser);
  return ret;
}
