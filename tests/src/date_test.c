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

  result = date_parse("2000-02-29", &date_result);
  CU_ASSERT_EQUAL(result, DATETIME_NO_ERROR);
}

void test_date_parse_feb_on_non_leap() {
  enum DateTimeError result;
  struct Date date_result;

  result = date_parse("1900-02-29", &date_result);

  CU_ASSERT_EQUAL(result, DATETIME_DAY_INVALID);
}

/*
 * Check that we get correct error codes for the different types of
 * invalid dates.
 */
void test_date_parse_correct_errors() {
  enum DateTimeError result;
  struct Date date_result;

  result = date_parse("2010-12-32", &date_result);
  CU_ASSERT_EQUAL(result, DATETIME_DAY_INVALID);

  result = date_parse("2010-13-32", &date_result);
  CU_ASSERT_EQUAL(result, DATETIME_MONTH_INVALID);

  result = date_parse("0-13-32", &date_result);
  CU_ASSERT_EQUAL(result, DATETIME_YEAR_INVALID);
}

void test_time_parse_valid_time() {
  enum DateTimeError result;
  struct Time time_result;

  result = time_parse("18:05", &time_result);
  CU_ASSERT_EQUAL(result, DATETIME_NO_ERROR);

  CU_ASSERT_EQUAL(time_result.hour, 18);
  CU_ASSERT_EQUAL(time_result.minutes, 5);
}

void test_time_parse_invalid_time() {
  enum DateTimeError result;
  struct Time time_result;

  result = time_parse("Adfdds", &time_result);
  CU_ASSERT_EQUAL(result, DATETIME_INVALID);

  CU_ASSERT_EQUAL(time_result.hour, 0);
  CU_ASSERT_EQUAL(time_result.minutes, 0);
}

void test_time_over_max_hours() {
  enum DateTimeError result;
  struct Time time_result;

  result = time_parse("25:00", &time_result);
  CU_ASSERT_EQUAL(result, DATETIME_HOUR_INVALID);
}

void test_time_no_minutes_over_max() {
  enum DateTimeError result;
  struct Time time_result;

  result = time_parse("24:01", &time_result);
  CU_ASSERT_EQUAL(result, DATETIME_MINUTES_INVALID);
}
