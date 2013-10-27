/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Functions for parsing and formatting dates.
 */

#include <stdio.h>
#include <string.h>

#include "bool.h"
#include "date_time.h"

#define NUM_MONTHS_IN_YEAR 12

/*
 * Forward declarations.
 */
static enum DateTimeError check_date_str_length(const char *const stDate);
static enum DateTimeError parse_date_string(const char *const stDate,
                                            struct Date *date);
static enum DateTimeError validate_date(int year, int month, int day);
static Boolean is_leap_year(int year);
static enum DateTimeError check_year(int year);
static enum DateTimeError check_month(int month);
static enum DateTimeError check_day(int year, int month, int day);

/*
 * Given a pointer to a string, will update the given date.
 * It will return the error status, on any error, the Date struct is reset
 * to 0-0-0.
 */
enum DateTimeError date_parse(const char *const stDate, struct Date *date) {
  enum DateTimeError error_result;
  error_result = check_date_str_length(stDate);

  if (error_result == DATETIME_NO_ERROR) {
    error_result = parse_date_string(stDate, date);
  }

  return error_result;
}

enum DateTimeError time_parse(const char *const stTime, struct Time *time) {
  return 0;
}

/*
 * Checks the length of the date string, and will return an error code
 * if it's too big or small.
 */
static enum DateTimeError check_date_str_length(const char *const stDate) {
  enum DateTimeError error_result;
  int date_str_len;

  error_result = DATETIME_NO_ERROR;
  date_str_len = strnlen(stDate, DATETIME_MAX_STR_LEN);

  if (date_str_len != DATETIME_MAX_STR_LEN) {
    if (date_str_len < DATETIME_MIN_STR_LEN) {
      error_result = DATETIME_STR_TOO_SHORT;
    }
  } else {
    error_result = DATETIME_STR_TOO_LONG;
  }

  return error_result;
}

/*
 * Parses the given string into the Date struct. Will return an error
 * if the string doesn't have a valid date, and will set the Date
 * struct to 0-0-0.
 */
static enum DateTimeError parse_date_string(const char *const stDate,
                                            struct Date *date) {
  enum DateTimeError error_result;
  int day, month, year, scan_result;

  date->year = 0;
  date->month = 0;
  date->day = 0;

  error_result = DATETIME_NO_ERROR;

  scan_result = sscanf(stDate, "%4d-%2d-%2d", &year, &month, &day);

  /* Haven't scanned three integers, then it's a problem. */
  if (scan_result != 3 || scan_result == EOF) {
    error_result = DATETIME_INVALID;
  } else {
    error_result = validate_date(year, month, day);
    if (error_result == DATETIME_NO_ERROR) {
      date->year = year;
      date->month = month;
      date->day = day;
    }
  }

  return error_result;
}

/*
 * The actual date validation.
 * Handle obvious bad dates, and if possible, invalid dates.
 */
static enum DateTimeError validate_date(int year, int month, int day) {
  enum DateTimeError result;

  result = check_year(year);
  if (result == DATETIME_NO_ERROR) {
    result = check_month(month);
    if (result == DATETIME_NO_ERROR) {
      result = check_day(year, month, day);
    }
  }

  return result;
}

/*
 * Return true if the given year is a leap year.
 */
static Boolean is_leap_year(int year) {
  Boolean result = FALSE;

  if (year % 400 == 0) {
    result = TRUE;
  } else if (year % 100 == 0) {
    result = FALSE;
  } else if (year % 4 == 0) {
    result = TRUE;
  } else {
    result = FALSE;
  }

  return result;
}

/*
 * Negative years are invalid, BC not covered.
 */
static enum DateTimeError check_year(int year) {
  enum DateTimeError result;
  result = DATETIME_NO_ERROR;

  if (year < 1) {
    result = DATETIME_YEAR_INVALID;
  }

  return result;
}

/*
 * Check the month is valid.
 */
static enum DateTimeError check_month(int month) {
  enum DateTimeError result;
  result = DATETIME_NO_ERROR;

  if ((month < 1) || (month > NUM_MONTHS_IN_YEAR)) {
    result = DATETIME_MONTH_INVALID;
  }

  return result;
}

/*
 * February is the weird one, number of days varies by leap year.
 */
static enum DateTimeError check_day(int year, int month, int day) {
  /*
   * 30 days hath September...
   * February has 28, unless it's a leap year.
   */
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int max_days;
  enum DateTimeError result;

  result = DATETIME_NO_ERROR;
  max_days = days_in_month[month];

  if (is_leap_year(year) && (month == 2)) {
    max_days = 29;
  }

  if (max_days < day) {
    result = DATETIME_DAY_INVALID;
  }

  return result;
}
