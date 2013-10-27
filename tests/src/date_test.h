#ifndef DATE_TEST_H_
#define DATE_TEST_H_

#include "date_time.h"

void test_date_parse_valid_date();

void test_date_parse_invalid_no_fields();

void test_date_parse_feb_on_leap();

void test_date_parse_feb_on_non_leap();

void test_date_parse_correct_errors();

void test_time_parse_valid_time();

void test_time_parse_invalid_time();

void test_time_over_max_hours();

void test_time_no_minutes_over_max();

#endif
