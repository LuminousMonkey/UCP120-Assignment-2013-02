#include <CUnit/CUnit.h>

#include "date_test.h"

void test_date_parse_valid_date() {
  enum DateTimeError result;
  struct Date date_result;

  result = date_parse("2010-05-24", &date_result);

  CU_ASSERT_EQUAL(result, DATETIME_NO_ERROR);
  CU_ASSERT_EQUAL(date_result.day, 24);
  CU_ASSERT_EQUAL(date_result.month, 5);
  CU_ASSERT_EQUAL(date_result.year, 2010);
}

void test_date_parse_invalid_no_fields() {
  enum DateTimeError result;
  struct Date date_result;

  result = date_parse("2010-04032", &date_result);

  CU_ASSERT_EQUAL(result, DATETIME_INVALID);
  CU_ASSERT_EQUAL(date_result.day, 0);
  CU_ASSERT_EQUAL(date_result.month, 0);
  CU_ASSERT_EQUAL(date_result.year, 0);
}

void test_date_parse_feb_on_leap() {
  enum DateTimeError result;
  struct Date date_result;

  result = date_parse("2016-02-29", &date_result);

  CU_ASSERT_EQUAL(result, DATETIME_NO_ERROR);
}
