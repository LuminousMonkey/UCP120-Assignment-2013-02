#include <CUnit/CUnit.h>

#include "date_test.h"

void testDateParseValidDate() {
  enum DateTimeError result;
  struct Date date_result;

  result = dateParse( "2010-05-24", &date_result );

  CU_ASSERT_EQUAL( result, DATETIME_NO_ERROR );
  CU_ASSERT_EQUAL( date_result.day, 24 );
  CU_ASSERT_EQUAL( date_result.month, 5 );
  CU_ASSERT_EQUAL( date_result.year, 2010 );
}

void testDateParseInvalidNoFields() {
  enum DateTimeError result;
  struct Date date_result;

  result = dateParse( "2010-04032", &date_result );

  CU_ASSERT_EQUAL( result, DATETIME_INVALID );
  CU_ASSERT_EQUAL( date_result.day, 0 );
  CU_ASSERT_EQUAL( date_result.month, 0 );
  CU_ASSERT_EQUAL( date_result.year, 0 );
}

void testDateParseFebOnLeap() {
  enum DateTimeError result;
  struct Date date_result;

  result = dateParse( "2016-02-29", &date_result );
  CU_ASSERT_EQUAL( result, DATETIME_NO_ERROR );

  result = dateParse( "2000-02-29", &date_result );
  CU_ASSERT_EQUAL( result, DATETIME_NO_ERROR );
}

void testDateParseFebOnNonLeap() {
  enum DateTimeError result;
  struct Date date_result;

  result = dateParse( "1900-02-29", &date_result );

  CU_ASSERT_EQUAL( result, DATETIME_DAY_INVALID );
}

/*
 * Check that we get correct error codes for the different types of
 * invalid dates.
 */
void testDateParseCorrectErrors() {
  enum DateTimeError result;
  struct Date date_result;

  result = dateParse( "2010-12-32", &date_result );
  CU_ASSERT_EQUAL( result, DATETIME_DAY_INVALID );

  result = dateParse( "2010-13-32", &date_result );
  CU_ASSERT_EQUAL( result, DATETIME_MONTH_INVALID );

  result = dateParse( "0-13-32", &date_result );
  CU_ASSERT_EQUAL( result, DATETIME_YEAR_INVALID );
}

void testDateStringOutput() {
  struct Date date;
  char result[MAX_DATE_STRING];

  date.day = 28;
  date.month = 10;
  date.year = 2010;

  dateString( result, &date );
  CU_ASSERT_STRING_EQUAL( "28 October 2010", result );

  date.day = 1;
  date.month = 1;
  date.year = 1582;

  dateString( result, &date );
  CU_ASSERT_STRING_EQUAL( "1 January 1582", result );
}

void testTimeParseValidTime() {
  enum DateTimeError result;
  struct Time time_result;

  result = timeParse( "18:05", &time_result );
  CU_ASSERT_EQUAL( result, DATETIME_NO_ERROR );

  CU_ASSERT_EQUAL( time_result.hour, 18 );
  CU_ASSERT_EQUAL( time_result.minutes, 5 );
}

void testTimeParseInvalidTime() {
  enum DateTimeError result;
  struct Time time_result;

  result = timeParse( "Adfdds", &time_result );
  CU_ASSERT_EQUAL( result, DATETIME_INVALID );

  CU_ASSERT_EQUAL( time_result.hour, 0 );
  CU_ASSERT_EQUAL( time_result.minutes, 0 );
}

void testTimeOverMaxHours() {
  enum DateTimeError result;
  struct Time time_result;

  result = timeParse( "25:00", &time_result );
  CU_ASSERT_EQUAL( result, DATETIME_HOUR_INVALID );
}

void testTimeNoMinutesOverMax() {
  enum DateTimeError result;
  struct Time time_result;

  result = timeParse( "24:01", &time_result );
  CU_ASSERT_EQUAL( result, DATETIME_MINUTES_INVALID );
}

void testTimeStringOutput() {
  struct Time time;
  char result[MAX_TIME_STRING];

  time.hour = 23;
  time.minutes = 59;

  timeString( result, &time );
  CU_ASSERT_STRING_EQUAL( "11:59pm", result );

  time.hour = 0;
  time.minutes = 0;

  timeString( result, &time );
  CU_ASSERT_STRING_EQUAL( "12am", result );

  time.hour = 12;
  time.minutes = 0;

  timeString( result, &time );
  CU_ASSERT_STRING_EQUAL( "12pm", result );

}
