#include "type.h"
#include <stdlib.h>

void destroy_name(db_name_t *p) {
  if (!p) {
    return;
  }
  destroy_name(p->next);
  free(p);
  p = 0;
}
