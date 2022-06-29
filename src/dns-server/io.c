#include "io.h"
#include "log.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

db_record_t *readfile(char *path, int *count, int *ret_code) {
  FILE *handle;
  size_t file_len;
  char *buf;
  handle = fopen(path, "rb");
  if (!handle) {
    log_error("cannot open hosts file\n");
    log_fatal("db cache init error");
    exit(-1);
  }
  fseek(handle, 0, SEEK_END);
  file_len = ftell(handle);

  buf = malloc(file_len * sizeof(char));
  rewind(handle);
  fread(buf, file_len, 1, handle);

  char *p = buf;
  char *end = p + file_len;
  db_record_t *rec = malloc(DB_PARSE_MAX_RECORD * sizeof(db_record_t));
  *count = 0;
  int code = DB_PARSE_RECORD_OKAY;
  while (code != DB_PARSE_RECORD_EOF) {
    code = db_parse_next_record(&p, end, rec + *count);
    if (code == DB_PARSE_RECORD_INVALID) {
      *ret_code = code;
      free(rec);
      return 0;
    }
    *count += 1;
  }
  *count -= 1;

  *ret_code = code;
  return rec;
}
