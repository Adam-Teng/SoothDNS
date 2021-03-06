#ifndef DNS_TYPE_H
#define DNS_TYPE_H

#include <stdint.h>

#define DN_MAXLABEL 32       // domain name max label number
#define DNS_MSG_MAX_LEN 512  // max byte length of a dns message
#define DNS_MSG_MAX_ENTRY 32 // max entry number of message

// QTYPE def
#define DNS_QTYPE_A 1
#define DNS_QTYPE_NS 2
#define DNS_QTYPE_MD 3
#define DNS_QTYPE_MF 4
#define DNS_QTYPE_CNAME 5
#define DNS_QTYPE_SOA 6
#define DNS_QTYPE_MB 7
#define DNS_QTYPE_MG 8
#define DNS_QTYPE_MR 9
#define DNS_QTYPE_NULL 10
#define DNS_QTYPE_WKS 11
#define DNS_QTYPE_PTR 12
#define DNS_QTYPE_HINFO 13
#define DNS_QTYPE_MINFO 14
#define DNS_QTYPE_MX 15
#define DNS_QTYPE_TXT 16
#define DNS_QTYPE_AAAA 28
#define DNS_QTYPE_AXFR 252
#define DNS_QTYPE_MAILB 253
#define DNS_QTYPE_MAILA 254
#define DNS_QTYPE_ASTERISK 255

// QCLASS def
#define DNS_QCLASS_IN 1
#define DNS_QCLASS_CS 2
#define DNS_QCLASS_CH 3
#define DNS_QCLASS_HS 4
#define DNS_QCLASS_ASTERISK 255

typedef uint16_t dns_size_t;

typedef struct dns_msg_header {
  uint16_t id;
  uint8_t qr;
  uint8_t opcode;
  uint8_t aa;
  uint8_t tc;
  uint8_t rd;
  uint8_t ra;
  uint8_t rcode;
  dns_size_t qd_cnt;
  dns_size_t an_cnt;
  dns_size_t ns_cnt;
  dns_size_t ar_cnt;
} dns_msg_header_t;

typedef struct dn_label {
  dns_size_t len;
  dns_size_t offset;
} dn_label_t;

typedef struct dn_name {
  dn_label_t labels[DN_MAXLABEL];
  dns_size_t len;
} dn_name_t;

typedef struct dns_msg_q {
  dn_name_t name;
  uint16_t type;
  uint16_t dclass;
} dns_msg_q_t;

typedef struct dns_msg_rr {
  dn_name_t name;
  uint16_t type;
  uint16_t dclass;
  uint16_t ttl;
  uint16_t rdlength;
  uint16_t data_offset;
} dns_msg_rr_t;

typedef struct dns_msg {
  char raw[DNS_MSG_MAX_LEN];
  uint32_t msg_len;
  uint32_t query_len;
  dns_msg_header_t header;
  dns_msg_q_t question[DNS_MSG_MAX_ENTRY];
  dns_msg_rr_t answer[DNS_MSG_MAX_ENTRY];
  dns_msg_rr_t authority[DNS_MSG_MAX_ENTRY];
  dns_msg_rr_t additional[DNS_MSG_MAX_ENTRY];
} dns_msg_t;

#endif // !DNS_TYPE_H
