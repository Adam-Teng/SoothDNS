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
#include "dns_parse.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

uint16_t get_dns_id(char *msg) { return ntohs(*(unsigned short *)msg); }

void set_dns_id(char *msg, uint16_t dns_id) {
  *(uint16_t *)msg = htons(dns_id);
}

char *parse_header(char *msg, dns_msg_header_t *header) {
  header->id = ntohs(*(unsigned short *)msg);
  msg += 2;

  uint8_t b = *(uint8_t *)msg;
  header->qr = (b >> 7) & 0x1;
  header->opcode = (b >> 3) & 0xf;
  header->aa = (b >> 2) & 0x1;
  header->tc = (b >> 1) & 0x1;
  header->rd = b & 0x1;
  msg += 1;

  b = *(uint8_t *)msg;
  header->ra = (b >> 7) & 0x1;
  header->rcode = b & 0xf;
  msg += 1;

  header->qd_cnt = ntohs(*(unsigned short *)msg);
  msg += 2;

  header->an_cnt = ntohs(*(unsigned short *)msg);
  msg += 2;

  header->ns_cnt = ntohs(*(unsigned short *)msg);
  msg += 2;

  header->ar_cnt = ntohs(*(unsigned short *)msg);
  msg += 2;

  return msg;
}

char *parse_label(char *current, char *orig_msg, dn_label_t *label,
                  int *ret_code) {
  uint16_t len = ntohs(*(uint16_t *)current);
  char *p = 0;
  char *ret = 0;
  if (((len >> 14) & 0x3) == 0x3) {
    uint16_t offset = len & 0x3fff;
    ret = current + 2;
    parse_label(orig_msg + offset, orig_msg, label, ret_code);
  } else if (((len >> 14) & 0x3) == 0x0) {
    p = current + 1;
    len = (*(uint8_t *)current);
    if (len == 0) {
      *ret_code = DN_PARSE_LABEL_EON;
      return p;
    }
    len &= 0x3f;
    label->offset = p - orig_msg;
    label->len = len;
    *ret_code = DN_PARSE_LABEL_OK;
    ret = p + len;
  } else {
    *ret_code = DN_PARSE_LABEL_INVALID_LEN;
    return 0;
  }
  return ret;
}

char *parse_name(char *current, char *orig_msg, dn_name_t *name,
                 int *ret_code) {
  char *p = current;
  int pl_ret_code = DN_PARSE_LABEL_OK;
  int len = 0;

  while (pl_ret_code == DN_PARSE_LABEL_OK) {
    p = parse_label(p, orig_msg, name->labels + len, &pl_ret_code);
    len = len + 1;
  }

  if (pl_ret_code == DN_PARSE_LABEL_INVALID_LEN) {
    *ret_code = DN_PARSE_NAME_INVALID;
    return 0;
  }

  len = len - 1;
  *ret_code = DN_PARSE_NAME_OK;
  name->len = len;

  return p;
}

char *parse_question(char *current, char *orig_msg, dns_msg_q_t *q,
                     int *ret_code) {
  char *p = current;
  int pn_code = DN_PARSE_NAME_INVALID;
  p = parse_name(p, orig_msg, &q->name, &pn_code);
  if (pn_code == DN_PARSE_NAME_INVALID) {
    *ret_code = DNS_MSG_PARSE_Q_INVALID;
    return 0;
  }
  q->type = ntohs(*(uint16_t *)p);
  p += 2;
  q->dclass = ntohs(*(uint16_t *)p);
  p += 2;
  *ret_code = DNS_MSG_PARSE_Q_OK;
  return p;
}

int parse_dns_msg(char *msg, dns_msg_t *res) {
  char *p = msg;
  p = parse_header(msg, &res->header);
  dns_size_t qcount = res->header.qd_cnt;

  for (dns_size_t i = 0; i < qcount; i++) {
    int pq_code = DNS_MSG_PARSE_Q_INVALID;
    p = parse_question(p, msg, res->question + i, &pq_code);
    if (pq_code == DNS_MSG_PARSE_Q_INVALID) {
      return DNS_MSG_PARSE_INVALID;
    }
  }

  res->msg_len = p - msg;
  return DNS_MSG_PARSE_OK;
}

char *compose_a_rr(dn_name_t *dn_name, int32_t ip, size_t *len) {
  char *rr = malloc(sizeof(char) * 16);
  assert(dn_name->len);
  *len = 16;
  //  设置domain名字
  *(uint16_t *)rr
    = htons((dn_name->labels[0].offset - 1) | 0xc000);
  char *p = rr;
  // 设置类型
  p += 2;
  *(uint16_t *)p = htons(DNS_QTYPE_A);
  // 设置class
  p += 2;
  *(uint16_t *)p = htons(DNS_QCLASS_IN);
  //设置ttl
  p += 2;
  *(uint16_t *)p = htons(2700);
  //设置rd长度
  p += 4;
  *(uint16_t *)p = htons(4);
  //设置ip
  p += 2;
  *(uint32_t *)p = htons(ip);

  return rr;
}

char *compose_a_rr_ans(char *raw, size_t raw_len, char *a_rr, size_t rr_len,
                       size_t *len) {
  *len = raw_len + rr_len;
  char *msg = malloc(sizeof(char)* * len);
  memcpy(msg, raw, raw_len);
  memcpy(msg + raw_len, a_rr, rr_len);

  //qr
  *(uint8_t *) (msg + 2) |= 0x80;
  //rcode
  *(uint8_t *) (msg + 3) &= 0xf0;
  //ans count
  *(uint16_t *) (msg + 6) = htons(1);
  //clear additional count
  *(uint16_t *) (msg + 10) = htons(0);

  return msg;
}

void print_qtype(uint16_t t) {
  switch (t) {
    case DNS_QTYPE_A:
      printf("A");
      break;
    case DNS_QTYPE_NS:
      printf("NS");
      break;
    case DNS_QTYPE_MD:
      printf("MD");
      break;
    case DNS_QTYPE_MF:
      printf("MF");
      break;
    case DNS_QTYPE_CNAME:
      printf("CNAME");
      break;
    case DNS_QTYPE_SOA:
      printf("SOA");
      break;
    case DNS_QTYPE_MB:
      printf("MB");
      break;
    case DNS_QTYPE_MG:
      printf("MG");
      break;
    case DNS_QTYPE_MR:
      printf("MR");
      break;
    case DNS_QTYPE_NULL:
      printf("NULL");
      break;
    case DNS_QTYPE_WKS:
      printf("WKS");
      break;
    case DNS_QTYPE_PTR:
      printf("PTR");
      break;
    case DNS_QTYPE_HINFO:
      printf("HINFO");
      break;
    case DNS_QTYPE_MINFO:
      printf("MINFO");
      break;
    case DNS_QTYPE_MX:
      printf("MX");
      break;
    case DNS_QTYPE_TXT:
      printf("TXT");
      break;
    case DNS_QTYPE_AXFR:
      printf("AXFR");
      break;
    case DNS_QTYPE_MAILB:
      printf("MAILB");
      break;
    case DNS_QTYPE_MAILA:
      printf("MAILA");
      break;
    case DNS_QTYPE_ASTERISK:
      printf("*");
      break;
    default:
      break;
  }
}
void print_qclass(uint16_t t) {
  switch (t) {
    case DNS_QCLASS_IN:
      printf("IN");
      break;
    case DNS_QCLASS_CS:
      printf("CS");
      break;
    case DNS_QCLASS_CH:
      printf("CH");
      break;
    case DNS_QCLASS_HS:
      printf("HS");
      break;
    case DNS_QCLASS_ASTERISK:
      printf("*");
      break;
    default:
      break;
  }
}

void print_question(const dns_msg_q_t *q, const char *raw_msg) {
  printf("Question: ");
  print_qclass(q->dclass);
  printf(" ");
  print_qtype(q->type);
  printf(" ");

  for (int i = 0; i < q->name.len; i++) {
    for (int j = 0; j < q->name.labels[i].len; j++) {
      int k = q->name.labels[i].offset + j;
      putchar(raw_msg[k]);
    }
    putchar('.');
  }
}


