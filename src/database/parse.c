#include "parse.h"
#include "log.h"
#include "type.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

db_ip_t _parse_ip(const char *str, uint16_t len, int *ret_code, db_ip_t access,
                  int depth);

db_name_t *db_parse_name(char *str, uint16_t len, int *ret_code) {
  if (!len) {
    *ret_code = DB_PARSE_NAME_OKAY;
    return 0;
  }

  db_name_t *ret = malloc(sizeof(db_name_t));
  char *p = str;
  while (*p != '.' && p != str + len) {
    p++;
  }
  int label_len = p - str;

  if (!label_len) {
    free(ret);
    *ret_code = DB_PARSE_NAME_INVALID;
    return 0;
  }

  ret->label.label = malloc(sizeof(char) * (label_len + 1));
  memcpy(ret->label.label, str, label_len);
  ret->label.label[label_len] = 0;
  ret->label.len = label_len;

  len = len - label_len;
  if (*p == '.') {
    p++;
    len--;
    if (!len) {
      *ret_code = DB_PARSE_NAME_INVALID;
      return 0;
    }
  }

  int code = 0;
  db_name_t *rest = db_parse_name(p, len, &code);
  if (code == DB_PARSE_NAME_INVALID) {
    *ret_code = code;
    return 0;
  }

  *ret_code = DB_PARSE_NAME_OKAY;
  ret->next = 0;
  for (int i = 0; ret->label.label[i]; i++) {
    ret->label.label[i] = tolower(ret->label.label[i]);
  }
  if (rest) {
    db_name_t *a = rest;
    while (a->next) {
      a = a->next;
    }
    a->next = ret;
    return rest;
  } else {
    return ret;
  }
}

db_ip_t db_parse_ip(const char *str, uint16_t len, int *ret_code) {
  return _parse_ip(str, len, ret_code, 0, 3);
}

uint16_t readint(const char *str, uint16_t len) {
  uint16_t ret = 0;
  for (const char *p = str; p != str + len; p++) {
    ret = ret * 10 + (*p) - '0';
  }
  return ret;
}

db_ip_t _parse_ip(const char *str, uint16_t len, int *ret_code, db_ip_t access,
                  int depth) {
  const char *p = str;
  while (*p != '.' && p != str + len) {
    if (!isdigit(*p)) {
      *ret_code = DB_PARSE_IP_INVALID;
      return 0;
    }
    p++;
  }

  if (depth == 0) {
    if (p != str + len || len == 0) {
      *ret_code = DB_PARSE_IP_INVALID;
      return 0;
    }
    *ret_code = DB_PARSE_IP_OKAY;
    return access * 256 + readint(str, len);
  }

  if (*p != '.') {
    *ret_code = DB_PARSE_IP_INVALID;
    return 0;
  }

  uint16_t slen = p - str;
  if (slen == 0) {
    *ret_code = DB_PARSE_IP_INVALID;
    return 0;
  }
  p++;
  len--;
  len -= slen;
  db_ip_t a = readint(str, slen);
  return _parse_ip(p, len, ret_code, access * 256 + a, depth - 1);
}

int db_parse_next_record(char **str, char *end, db_record_t *res) {
  char *p = *str;
  while ((*p == '\n' || *p == '\r') && p != end) {
    p++;
  }
  if (p == end) {
    *str = end;
    return DB_PARSE_RECORD_EOF;
  }

  char *a = p;
  while (*a != '\n' && *a != '\r' && a != end) {
    a++;
  }

  char *b = p;
  while (*b != ' ' && b != end) {
    b++;
  }
  if (b == end || b == end - 1) {
    *str = end;
    return DB_PARSE_RECORD_INVALID;
  }

  int code = -1;
  res->ip = db_parse_ip(p, b - p, &code);
  if (code == DB_PARSE_IP_INVALID) {
    return DB_PARSE_RECORD_INVALID;
  }

  res->name = db_parse_name(b + 1, a - b - 1, &code);
  if (code == DB_PARSE_NAME_INVALID) {
    return DB_PARSE_RECORD_INVALID;
  }
  *str = a;
  return DB_PARSE_RECORD_OKAY;
}
