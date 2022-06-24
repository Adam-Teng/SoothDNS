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

#include "args.h"
#include "colors.h"
#include "log.h"
#include "server.h"

static parameter_t* para = 0;

int main(int argc, char* argv[]) {
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
	log_info("remote server: %s", para->server_addr);
	log_info("max query: %d", para->max_query);
	log_info("max udp req: %d", para->max_udp_req);
	log_info("client port: %d", para->client_port);
	log_info("hosts file: %s", para->hosts_paths);

	return EXIT_SUCCESS;
}
