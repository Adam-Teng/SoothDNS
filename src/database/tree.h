#ifndef DB_TREE_H
#define DB_TREE_H

#include "parse.h"
#include <stdlib.h>

struct tree_node_s {
  char *key;
  db_record_t *rec;
  struct tree_node_s *children[256];
  size_t len;
};
typedef struct tree_node_s tree_node_t;

typedef tree_node_t *tree_t;

tree_t *tree_init();

void tree_insert_rec(tree_t *tree, db_record_t *rec);

db_record_t *tree_lookup(tree_t *tree, db_name_t *name);

tree_t *tree_build_from_rec(db_record_t *rec, size_t len);

void free_tree(tree_t *tree);

#endif
