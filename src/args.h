/*
 * ============================================================================
 *
 *       Filename:  args.h
 *
 *    Description:  Header file of the command line options parser
 *
 *        Created:  24/03/2016 21:30:39 PM
 *       Compiler:  gcc
 *
 *         Author:  Gustavo Pantuza
 *   Organization:  Software Community
 *
 * ============================================================================
 */

#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>
#include <stdbool.h>
#include <string.h>

/* Max size of a file name */
#define FILE_NAME_SIZE 512

/* Defines the command line allowed options struct */
struct options {
  bool help;
  bool version;
  bool use_colors;
  char file_name[FILE_NAME_SIZE];
};

/* Exports options as a global type */
typedef struct options options_t;

/* Defines the parameter for the program */
struct parameter {
  char *server_addr;
  int max_query;
  int max_udp_req;
  int client_port;
  char *hosts_paths;
};

/* Exports parameter as a global type */
typedef struct parameter parameter_t;

/* Public functions section */
parameter_t *parameter_init();

/* Public functions section */
void options_parser(int argc, char *argv[], options_t *options);

#endif // ARGS_H
