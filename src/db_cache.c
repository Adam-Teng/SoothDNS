#include "db_cache.h"

#include <stdlib.h>
#include <string.h>

#include "db_type.h"

uint16_t hash_name(db_name_t* name) {
	if (!name) return 0;
	uint16_t res = 0;
	for (size_t i = 0; i < name->label.len; i++) {
		res += res * 31 + name->label.label[i];
	}
	return res ^ hash_name(name->next);
}

cache_t* db_cache_init() {
	cache_t* c = malloc(sizeof(cache_t));
	memset(c, 0, sizeof(cache_t));
	return c;
}

void db_cache_free(cache_t* c) {
	for (size_t i = 0; i < DB_CACHE_WAY_NUM; i++) {
		for (size_t j = 0; j < DB_CACHE_ENTRY_NUM; j++) {
			destroy_name(c->entry[i][j].name);
		}
	}
	free(c);
}

db_name_t* dup_name(db_name_t* name) {
	if (!name) return 0;
	db_name_t* ret = malloc(sizeof(db_name_t));
	ret->label.len = name->label.len;
	ret->label.label = strdup(name->label.label);
	ret->next = dup_name(name->next);
	return ret;
}

void db_cache_insert(cache_t* c, db_name_t* name, db_record_t* rec) {
	uint16_t idx = hash_name(name);
	char sel = c->lru[idx];
	c->entry[sel][idx].name = dup_name(name);
	c->entry[sel][idx].rec = rec;
	c->lru[idx] = sel == 0 ? 1 : 0;
}

int compare_name(db_name_t* a, db_name_t* b) {
	if (a == 0 && b == 0) {
		return 0;
	}
	if (a == 0 || b == 0) {
		return -1;
	}
	if (strcmp(a->label.label, b->label.label) == 0) {
		return compare_name(a->next, b->next);
	}
	return -1;
}

db_record_t* db_cache_lookup(cache_t* c, db_name_t* name) {
	uint16_t idx = hash_name(name);
	for (size_t i = 0; i < DB_CACHE_ENTRY_NUM; i++)
	{
		if (compare_name(name, c->entry[i][idx].name) == 0)
		{
			c->lru[idx] = i == 0 ? 1 : 0;
			return c->entry[i][idx].rec;
		}
	}
	return 0;
}
