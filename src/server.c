#include "server.h"
#include "log.h"

uv_loop_t* loop = 0;
tree_t* db_tree = 0;
cache_t* db_cache = 0;
static db_record_t* db_rec = 0;

void loop_init() { loop = uv_default_loop(); }

void cache_init(parameter_t* para) {
	int count = 0;
	int db_rec_code = 0;
	db_rec = db_io("~/GitRepo/misc/dns-relay/hosts.txt", &count, &db_rec_code);
	if (db_rec_code != DB_PARSE_RECORD_EOF) {
		log_fatal("error parsing hosts file.");
		exit(-1);
	}
	db_tree = tree_build_from_rec(db_rec, count);
	db_cache = db_cache_init();
}

void cache_deinit() { free(db_rec); }

void server_init(parameter_t* para) {
	loop_init();
	log_info("libuv event loop initialized");
	//  pools_init(para);
	log_info("pools initialized");
	cache_init(para);
	log_info("hosts db and cache initialized");
}

int server_run() { return 0; }

void server_deinit() {
	cache_deinit();
	//  pools_deinit();
}
