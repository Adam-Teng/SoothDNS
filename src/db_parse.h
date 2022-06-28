/*
 * ============================================================================
 *
 *       Filename:  db_parse.h
 *
 *    Description:  header file for db_parse.c
 *
 *        Created:  05/14/2022
 *
 *         Author:  yhteng
 *
 * ============================================================================
 */
#ifndef DB_PARSE_H
#define DB_PARSE_H

#include "db_type.h"

#define DB_PARSE_NAME_OKAY 0
#define DB_PARSE_NAME_INVALID 1

#define DB_PARSE_IP_OKAY 0
#define DB_PARSE_IP_INVALID 1

#define DB_PARSE_RECORD_OKAY 0
#define DB_PARSE_RECORD_INVALID 1
#define DB_PARSE_RECORD_EOF 2

#define DB_PARSE_MAX_RECORD 2048

db_name_t *db_parse_name(char *str, uint16_t len, int *ret_code);
db_ip_t db_parse_ip(const char *str, uint16_t len, int *ret_code);
int db_parse_next_record(char **p, char *end, db_record_t *res);

db_record_t *db_io(char *path, int *count, int *ret_code);

#endif // !DB_PARSE_H
