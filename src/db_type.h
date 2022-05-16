#ifndef DB_TYPE_H
#define DB_TYPE_H

#include <stdint.h>

#define DN_LABEL_MAX_LEN 64
#define DN_NAME_MAX_LEN 256

typedef uint16_t db_size_t;
typedef uint32_t db_ip_t;

typedef struct db_label {
  db_size_t len;
  char *label;
} db_label_t;

typedef struct db_name {
  db_label_t label;
  struct db_name *next;
} db_name_t;

typedef struct db_record {
  db_name_t *name;
  db_ip_t ip;
} db_record_t;

void destroy_name(db_name_t *p);

#endif
