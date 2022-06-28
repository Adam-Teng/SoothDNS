/*
 * ============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main file of the project
 *
 *        Created:  05/15/2022
 *
 *         Author:  yhteng
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "dns-server/server.h"
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

  log_info("dns relay started");

  /* Print basic information */
  log_info("remote server: %s", para->server_addr);
  log_info("max query: %d", para->max_query);
  log_info("max udp req: %d", para->max_udp_req);
  log_info("client port: %d", para->client_port);
  log_info("hosts file: %s", para->host_path);

  /* prints parser's state */
  ap_print(parser);

  /* init server */
  loop_init();
  log_info("libuv event loop initialized");
  socket_init();
  log_info("udp socket initialized");
  log_info("server initialized, listening at port 53");

  /* run server */
  server_run();

  /* Free the parser's memory */
  ap_free(parser);
  return EXIT_SUCCESS;
}
