/*
 * ============================================================================
 *
 *       Filename:  dns_parse.c
 *
 *    Description:  对dns报文进行解析
 *
 *        Created:  05/14/22
 *
 *         Author:  yhteng
 *
 * ============================================================================
 */

#ifndef DNS_PARSE_H
#define DNS_PARSE_H

#include "dns_type.h"
#include <arpa/inet.h>
#include <stdlib.h>

#define DN_PARSE_LABEL_OK 0
#define DN_PARSE_LABEL_INVALID_LEN 1
#define DN_PARSE_LABEL_EON 2

#define DN_PARSE_NAME_OK 0
#define DN_PARSE_NAME_INVALID 1

#define DNS_MSG_PARSE_Q_OK 0
#define DNS_MSG_PARSE_Q_INVALID 1

#define DNS_MSG_PARSE_OK 0
#define DNS_MSG_PARSE_INVALID 1

char *parse_header(char *msg, dns_msg_header_t *header);
char *parse_name(char *current, char *orig_msg, dn_name_t *name, int *ret_code);
char *parse_question(char *current, char *orig_msg, dns_msg_q_t *q,
                     int *ret_code);
int parse_dns_msg(char *msg, dns_msg_t *res);

uint16_t get_dns_id(char *msg);
void set_dns_id(char *msg, uint16_t dns_id);

char *compose_a_rr(dn_name_t *dn_name, int32_t ip, size_t *len);
char *compose_a_rr_ans(char *raw, size_t raw_len, char *a_rr, size_t rr_len,
                       size_t *len);

void print_qtype(uint16_t t);
void print_qclass(uint16_t t);
void print_question(const dns_msg_q_t *q, const char *raw_msg);

#endif // !DNS_PARSE_H
