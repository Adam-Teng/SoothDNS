/*
 * ============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main file of the project
 *
 *        Created:  03/24/2016 19:40:56
 *
 *         Author:  Gustavo Pantuza
 *   Organization:  Software Community
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "colors.h"
#include "log.h"
#include "server.h"

static parameter_t *para = 0;

int main(int argc, char *argv[]) {

  /* Init parameters */
  para = parameter_init();

  /* Read command line options */
  options_t options;
  options_parser(argc, argv, &options);

#ifdef DEBUG
  fprintf(stdout, BLUE "Command line options:\n" NO_COLOR);
  fprintf(stdout, BROWN "help: %d\n" NO_COLOR, options.help);
  fprintf(stdout, BROWN "version: %d\n" NO_COLOR, options.version);
  fprintf(stdout, BROWN "use colors: %d\n" NO_COLOR, options.use_colors);
  fprintf(stdout, BROWN "filename: %s\n" NO_COLOR, options.file_name);
#endif

  log_info("dns relay started");

  /* Print basic information */
  log_info("remote server: %s, max query: %d, max udp req: %d, client port: %d, hosts file: %s",
           para->server_addr, para->max_query, para->max_udp_req, para->client_port,
           para->hosts_paths);

  return EXIT_SUCCESS;
}
