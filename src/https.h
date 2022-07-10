#ifndef HTTPS_H
#define HTTPS_H

#include "args.h"
#include "type.h"
#include <curl/curl.h>
#include <uv.h>

#define HTTPS_TIMEOUT 8000

void add_doh_connection(struct sockaddr addr, char *req_data, size_t req_len,
                        db_name_t *name);

void curl_init();

void curl_deinit();

#endif
