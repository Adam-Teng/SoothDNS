/*
 * ============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main execution of the tests using cmocka
 *
 *        Created:  04/28/2016 19:21:37
 *       Compiler:  gcc
 *
 *         Author:  Gustavo Pantuza
 *   Organization:  Software Community
 *
 * ============================================================================
 */

#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

/* include here your files that contain test functions */

#include "database/parse.c"
#include "database/parse.h"
#include "database/type.c"
#include "database/type.h"

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {

  /**
   * If you want to know how to use cmocka, please refer to:
   * https://api.cmocka.org/group__cmocka__asserts.html
   */
  (void)state; /* unused */
}

/**
 * Test runner function
 */

static void test_destroy_name_null_returnnull() {
  db_name_t *p = 0;
  destroy_name(p);
  assert_null(p);
}

static void test_db_parse_name_correct1() {
  int code = -1;
  db_name_t *name = db_parse_name("www.google.com", 14, &code);
  db_name_t *p = name;

  assert_non_null(p);
  assert_string_equal("com", p->label.label);
  assert_int_equal(3, p->label.len);

  p = p->next;
  assert_non_null(p);
  assert_string_equal("google", p->label.label);
  assert_int_equal(6, p->label.len);

  p = p->next;
  assert_non_null(p);
  assert_string_equal("www", p->label.label);
  assert_int_equal(3, p->label.len);

  p = p->next;
  assert_null(p);
}

static void test_db_parse_name_correct_2() {
  int code = -1;
  db_name_t *name = db_parse_name("www.GOOGLE.c0m", 14, &code);
  db_name_t *p = name;

  assert_non_null(p);
  assert_int_equal(code, DB_PARSE_NAME_OKAY);
  assert_string_equal("c0m", p->label.label);
  assert_int_equal(3, p->label.len);

  p = p->next;
  assert_non_null(p);
  assert_string_equal("google", p->label.label);
  assert_int_equal(6, p->label.len);

  p = p->next;
  assert_non_null(p);
  assert_string_equal("www", p->label.label);
  assert_int_equal(3, p->label.len);

  p = p->next;
  assert_null(p);
}

static void test_db_parse_name_null() {
  int code = -1;
  db_name_t *name = db_parse_name("", 0, &code);

  assert_null(name);
  assert_int_equal(code, DB_PARSE_NAME_OKAY);
}

static void test_db_parse_name_invalid() {
  int code = -1;
  db_name_t *name = db_parse_name("..", 2, &code);

  assert_null(name);
  assert_int_equal(code, DB_PARSE_NAME_INVALID);

  name = db_parse_name("google.", 7, &code);
  assert_null(name);
  assert_int_equal(code, DB_PARSE_NAME_INVALID);
}

static void test_db_parse_ip_correct() {
  int code1 = -1;
  int code2 = -1;
  int code3 = -1;
  int code4 = -1;

  char a[] = "0.0.0.1";
  db_ip_t ip = db_parse_ip(a, strlen(a), &code1);
  assert_int_equal(code1, DB_PARSE_IP_OKAY);
  assert_int_equal(ip, 1);

  char b[] = "1.1.1.1";
  ip = db_parse_ip(b, strlen(b), &code2);
  assert_int_equal(code2, DB_PARSE_IP_OKAY);
  assert_int_equal(ip, 16843009);

  char c[] = "4.3.2.1";
  ip = db_parse_ip(c, strlen(c), &code3);
  assert_int_equal(code3, DB_PARSE_IP_OKAY);
  assert_int_equal(ip, 67305985);

  char d[] = "10.0.0.000";
  ip = db_parse_ip(d, strlen(d), &code4);
  assert_int_equal(code4, DB_PARSE_IP_OKAY);
  assert_int_equal(ip, 167772160);
}

static void test_db_parse_ip_invalid_end() {
  int code = -1;

  char a[] = "0.0.0.";
  db_ip_t ip = db_parse_ip(a, strlen(a), &code);
  assert_int_equal(code, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);
}

static void test_db_parse_ip_invalid_mid() {
  int code = -1;
  char a[] = "0..1.1";
  db_ip_t ip = db_parse_ip(a, strlen(a), &code);
  assert_int_equal(code, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);
}

static void test_db_parse_ip_invalid_notdigit() {
  int code1 = -1;
  int code2 = -1;
  int code3 = -1;
  int code4 = -1;

  char a[] = "0.0.0.1a";
  db_ip_t ip = db_parse_ip(a, strlen(a), &code1);
  assert_int_equal(code1, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);

  char b[] = "1a.0.0.1";
  ip = db_parse_ip(b, strlen(b), &code2);
  assert_int_equal(code2, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);

  char c[] = "a1.0.0.1";
  ip = db_parse_ip(c, strlen(c), &code3);
  assert_int_equal(code3, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);

  char d[] = "1.0.abc.1";
  ip = db_parse_ip(d, strlen(d), &code4);
  assert_int_equal(code4, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);
}

static void test_db_parse_ip_depth() {
  int code = -1;

  char a[] = "0";
  db_ip_t ip = db_parse_ip(a, strlen(a), &code);
  assert_int_equal(code, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);

  char b[] = "0.0";
  ip = db_parse_ip(b, strlen(b), &code);
  assert_int_equal(code, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);

  char c[] = "1.2.3.4.5";
  ip = db_parse_ip(c, strlen(c), &code);
  assert_int_equal(code, DB_PARSE_IP_INVALID);
  assert_int_equal(ip, 0);
}

static void test_db_parse_next_record_1() {
  char s[] = "\n\n1.2.3.4 www.google.com\n\n";
  char *p = s;
  db_record_t res;
  int code = db_parse_next_record(&p, s + strlen(s), &res);
  assert_int_equal(code, DB_PARSE_RECORD_OKAY);

  char sn[] = "www.google.com";
  db_name_t *name = db_parse_name(sn, strlen(sn), &code);
  for (db_name_t *x = name, *y = res.name; x != 0 && y != 0;
       x = x->next, y = y->next) {
    assert_string_equal(x->label.label, y->label.label);
  }

  char si[] = "1.2.3.4";
  db_ip_t ip = db_parse_ip(si, strlen(si), &code);
  assert_int_equal(ip, res.ip);
}

static void test_db_parse_next_record_2() {
  char s[] = "\n\n\n172.24.32.1 @.localhost";
  char *p = s;
  db_record_t res;
  int code = db_parse_next_record(&p, s + strlen(s), &res);
  assert_int_equal(code, DB_PARSE_RECORD_OKAY);

  char sn[] = "@.localhost";
  db_name_t *name = db_parse_name(sn, strlen(sn), &code);
  for (db_name_t *x = name, *y = res.name; x != 0 && y != 0;
       x = x->next, y = y->next) {
    assert_string_equal(x->label.label, y->label.label);
  }

  char si[] = "172.24.32.1";
  db_ip_t ip = db_parse_ip(si, strlen(si), &code);
  assert_int_equal(ip, res.ip);
}

int main(void) {

  /**
   * Insert here your test functions
   */
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(null_test_success),
      cmocka_unit_test(test_destroy_name_null_returnnull),
      cmocka_unit_test(test_db_parse_name_correct1),
      cmocka_unit_test(test_db_parse_name_correct_2),
      cmocka_unit_test(test_db_parse_name_null),
      cmocka_unit_test(test_db_parse_name_invalid),
      cmocka_unit_test(test_db_parse_ip_correct),
      cmocka_unit_test(test_db_parse_ip_invalid_end),
      cmocka_unit_test(test_db_parse_ip_invalid_mid),
      cmocka_unit_test(test_db_parse_ip_invalid_notdigit),
      cmocka_unit_test(test_db_parse_ip_depth),
      cmocka_unit_test(test_db_parse_next_record_1),
      cmocka_unit_test(test_db_parse_next_record_2),
  };

  /* Run the tests */
  return cmocka_run_group_tests(tests, NULL, NULL);
}
