/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Functions for parsing and formatting dates.
 */

#include <stdio.h>
#include <string.h>

#include "date_time.h"

/*
 * Given a pointer to a string, will update the given date.
 * It will return the error status, on any error, the Date struct is reset
 * to 0-0-0.
 */
enum DateTimeError date_parse(const char const *stDate, struct Date *date) {
  enum DateTimeError error_result;
  error_result = check_date_str_length(stDate);

  if (error_result == DATETIME_NO_ERROR) {
    error_result = parse_date_string(stDate, date);
  }

  return error_result;
}

enum DateTimeError time_parse(const char const *stTime, struct Time *time);

/*
 * Checks the length of the date string, and will return an error code
 * if it's too big or small.
 */
static enum DateTimeError check_date_str_length(const char const *stDate) {
  enum DateTimeError error_result;
  int date_str_len;

  error_result = DATETIME_NO_ERROR;
  date_str_len = strnlen(stDate, DATE_TIME_MAX_STR_LEN);

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
static enum DateTimeError parse_date_string(const char const *stDate,
                                            struct Date *date) {
  enum DateTimeError error_result;
  int day, month, year, scan_result;

  error_result = DATETIME_NO_ERROR;

  scan_result = sscanf(date, "%4d-%2d-%2d", &year, &month, &day);

  /* Haven't scanned three integers, then it's a problem. */
  if (scan_result != 3 || scan_result == EOF) {

  }

  return error_result;
}
