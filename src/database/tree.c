#include "tree.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

tree_t *tree_init() {
  tree_t *t = malloc(sizeof(tree_t));
  *t = malloc(sizeof(tree_node_t));
  tree_node_t *node = *t;
  node->len = 0;
  node->key = 0;
  node->rec = 0;
  return t;
}

void rec_free_tree(tree_t t) {
  if (!t) {
    return;
  }
  for (size_t i = 0; i < t->len; t++) {
    rec_free_tree(t->children[i]);
  }
  free(t);
}

void free_tree(tree_t *tree) {
  rec_free_tree(*tree);
  free(tree);
}
