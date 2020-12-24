#include <check.h>

#include "parse_sender.h"

static void assert_parse_sender(char *input, size_t expected_exit, ssize_t expected_return, char *expected_user, char *expected_host) {
  size_t retval = parse_sender(input);

  ck_assert_int_eq(expected_exit, exitcode);
  if (exitcode) {
    exitcode = 0;
    return;
  }

  ck_assert_int_eq(expected_return, retval);
  ck_assert_str_eq(expected_user,   getenv("QMAILUSER"));
  ck_assert_str_eq(expected_host,   getenv("QMAILHOST"));
}

START_TEST(test_parse_sender)
{
  /*
   * bruceg: "I can't see any reason for a segfault
   * unless the envelope file is missing a terminating NUL byte."
   */

  assert_parse_sender(NULL,                91, -77, "dummy", "unused");
  //assert_parse_sender('F',                  0, -77, "dummy", "unused");
  assert_parse_sender('Q',                  0, -77, "dummy", "unused");
  assert_parse_sender("",                  91, -77, "dummy", "unused");
  assert_parse_sender("F",                  0,   2, "", "");
  assert_parse_sender("Q",                 91, -88, "dummy", "unused");
  assert_parse_sender("Fnoat",              0,   6, "noat", "");
  assert_parse_sender("F@",                 0,   3, "", "");
  assert_parse_sender("Fbefore@",           0,   9, "before", "");
  assert_parse_sender("F@after",            0,   8, "", "after");
  assert_parse_sender("Fbefore@after",      0,  14, "before", "after");
  assert_parse_sender("Fbefore@both@after", 0,  19, "before@both", "after");

  char maybe_unterminated[] = {'F','b','@','a'};
  assert_parse_sender(maybe_unterminated,   0,   5, "b", "a");
}
END_TEST

TCase
*parse_sender_something(void)
{
  TCase *tc = tcase_create("basic operations");

  tcase_add_test(tc, test_parse_sender);

  return tc;
}

Suite
*parse_sender_suite(void)
{
  Suite *s = suite_create("qmail-qfilter parse_sender");

  suite_add_tcase(s, parse_sender_something());

  return s;
}

int
main(void)
{
  int number_failed;

  SRunner *sr = srunner_create(parse_sender_suite());
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed;
}
