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

#define DN_PARSE_LABEL_OKAY 0
#define DN_PARSE_LABEL_INVALID_LEN 1
#define DN_PARSE_LABEL_EON 2
#define DN_PARSE_LABEL_OKAY_PTR 3

#define DN_PARSE_NAME_OKAY 0
#define DN_PARSE_NAME_INVALID 1

#define DNS_MSG_PARSE_Q_OKAY 0
#define DNS_MSG_PARSE_Q_INVALID 1

#define DNS_MSG_PARSE_RR_OKAY 0
#define DNS_MSG_PARSE_RR_INVALID 1

#define DNS_MSG_PARSE_OKAY 0
#define DNS_MSG_PARSE_INVALID 1

char *parse_header(char *msg, dns_msg_header_t *header);
char *parse_name(char *current, char *orig_msg, dn_label_t *label,
                 dns_size_t *name_len, int *ret_code);
char *parse_question(char *current, char *orig_msg, dns_msg_q_t *q,
                     int *ret_code);
char *parse_rr(char *current, char *orig_msg, dns_msg_rr_t *rr, int *ret_code);
int parse_dns_msg(char *msg, dns_msg_t *res);

uint16_t get_dns_id(char *msg);
void set_dns_id(char *msg, uint16_t dns_id);

char *compose_a_rr(dn_name_t *dn_name, int32_t ip, uint32_t ttl, size_t *len);
char *compose_a_rr_ans(char *raw, size_t raw_len, char *a_rr, size_t rr_len,
                       size_t *len);
char *compose_header(dns_msg_header_t *header, size_t *msg_len);

void print_qtype(uint16_t t);
void print_qclass(uint16_t t);

void print_question(const dns_msg_q_t *q, const char *raw_msg);

void print_dns_msg(dns_msg_t *msg);
void print_dns_header(dns_msg_header_t *header);
void print_dns_domain_name(dn_name_t *name, char *orig_msg);
void print_dns_question(dns_msg_q_t *q, char *orig_msg);
void print_dns_rr(dns_msg_rr_t *rr, char *orig_msg);

#endif // !DNS_PARSE_H
