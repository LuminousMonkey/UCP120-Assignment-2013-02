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

#define MAX_HOURS 24
#define MAX_MINUTES 59

/*
 * Forward declarations.
 */
static enum DateTimeError checkStrLength( const char *const stDate,
    const size_t min,
    const size_t max );
static enum DateTimeError parseDateString( const char *const stDate,
    struct Date *date );
static enum DateTimeError validateDate( int year, int month, int day );
static Boolean isLeapYear( int year );
static enum DateTimeError checkYear( int year );
static enum DateTimeError checkMonth( int month );
static enum DateTimeError checkDay( int year, int month, int day );
static enum DateTimeError parseTimeString( const char *const stTime,
    struct Time *time );
static enum DateTimeError validateTime( int hour, int minutes );

/*
 * Given a pointer to a string, will update the given date.
 * It will return the error status, on any error, the Date struct is reset
 * to 0-0-0.
 */
enum DateTimeError dateParse( const char *const stDate, struct Date *date ) {
  enum DateTimeError error_result;
  error_result = checkStrLength( stDate, DATETIME_MIN_DATE_STR_LEN,
                                 DATETIME_MAX_DATE_STR_LEN );

  if ( error_result == DATETIME_NO_ERROR ) {
    error_result = parseDateString( stDate, date );
  }

  return error_result;
}

enum DateTimeError timeParse( const char *const stTime, struct Time *time ) {
  enum DateTimeError error_result;
  error_result = checkStrLength( stTime, DATETIME_MIN_TIME_STR_LEN,
                                 DATETIME_MAX_TIME_STR_LEN );

  if ( error_result == DATETIME_NO_ERROR ) {
    error_result = parseTimeString( stTime, time );
  }

  return error_result;
}

/*
 * Checks the length of the date string, and will return an error code
 * if it's too big or small.
 */
static enum DateTimeError checkStrLength( const char *const stDate,
    const size_t min,
    const size_t max ) {
  enum DateTimeError error_result;
  size_t date_str_len;

  error_result = DATETIME_NO_ERROR;
  date_str_len = strnlen( stDate, max );

  if ( date_str_len != max ) {
    if ( date_str_len < min ) {
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
static enum DateTimeError parseDateString( const char *const stDate,
    struct Date *date ) {
  enum DateTimeError error_result;
  int day, month, year, scan_result;

  date->year = 0;
  date->month = 0;
  date->day = 0;

  error_result = DATETIME_NO_ERROR;

  scan_result = sscanf( stDate, "%4d-%2d-%2d", &year, &month, &day );

  /* Haven't scanned three integers, then it's a problem. */
  if ( scan_result != 3 || scan_result == EOF ) {
    error_result = DATETIME_INVALID;
  } else {
    error_result = validateDate( year, month, day );

    if ( error_result == DATETIME_NO_ERROR ) {
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
static enum DateTimeError validateDate( int year, int month, int day ) {
  enum DateTimeError result;

  result = checkYear( year );

  if ( result == DATETIME_NO_ERROR ) {
    result = checkMonth( month );

    if ( result == DATETIME_NO_ERROR ) {
      result = checkDay( year, month, day );
    }
  }

  return result;
}

/*
 * Return true if the given year is a leap year.
 */
static Boolean isLeapYear( int year ) {
  Boolean result = FALSE;

  if ( year % 400 == 0 ) {
    result = TRUE;
  } else if ( year % 100 == 0 ) {
    result = FALSE;
  } else if ( year % 4 == 0 ) {
    result = TRUE;
  } else {
    result = FALSE;
  }

  return result;
}

/*
 * Negative years are invalid, BC not covered.
 */
static enum DateTimeError checkYear( int year ) {
  enum DateTimeError result;
  result = DATETIME_NO_ERROR;

  if ( year < 1 ) {
    result = DATETIME_YEAR_INVALID;
  }

  return result;
}

/*
 * Check the month is valid.
 */
static enum DateTimeError checkMonth( int month ) {
  enum DateTimeError result;
  result = DATETIME_NO_ERROR;

  if ( ( month < 1 ) || ( month > NUM_MONTHS_IN_YEAR ) ) {
    result = DATETIME_MONTH_INVALID;
  }

  return result;
}

/*
 * February is the weird one, number of days varies by leap year.
 *
 * It is assumed that month verification has been done.
 */
static enum DateTimeError checkDay( int year, int month, int day ) {
  /*
   * 30 days hath September...
   * February has 28, unless it's a leap year.
   */
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int max_days;
  enum DateTimeError result;

  result = DATETIME_NO_ERROR;
  max_days = days_in_month[month - 1];

  if ( isLeapYear( year ) && ( month == 2 ) ) {
    max_days = 29;
  }

  if ( max_days < day ) {
    result = DATETIME_DAY_INVALID;
  }

  return result;
}

static enum DateTimeError parseTimeString( const char *const stTime,
    struct Time *time ) {
  enum DateTimeError error_result;
  int hour, minutes, scan_result;

  time->hour = 0;
  time->minutes = 0;

  error_result = DATETIME_NO_ERROR;

  scan_result = sscanf( stTime, "%2d:%2d", &hour, &minutes );

  /* Haven't scanned two integers. */
  if ( scan_result != 2 || scan_result == EOF ) {
    error_result = DATETIME_INVALID;
  } else {
    error_result = validateTime( hour, minutes );

    if ( error_result == DATETIME_NO_ERROR ) {
      time->hour = hour;
      time->minutes = minutes;
    }
  }

  return error_result;
}

static enum DateTimeError validateTime( int hour, int minutes ) {
  enum DateTimeError result;

  result = DATETIME_NO_ERROR;

  if ( ( hour > MAX_HOURS ) || ( hour < 0 ) ) {
    result = DATETIME_HOUR_INVALID;
  } else if ( ( MAX_MINUTES > 59 ) || ( minutes < 0 ) ) {
    result = DATETIME_MINUTES_INVALID;
  } else if ( ( hour == MAX_HOURS ) && ( minutes > 0 ) ) {
    result = DATETIME_MINUTES_INVALID;
  }

  return result;
}
