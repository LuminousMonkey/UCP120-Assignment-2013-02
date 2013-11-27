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

/*
 * The max and min length we expect for date and time strings.
 * Any string that is greater than the MAX is too long.
 */
#define DATETIME_MIN_DATE_STR_LEN 6
#define DATETIME_MAX_DATE_STR_LEN 20
#define DATETIME_MIN_TIME_STR_LEN 3
#define DATETIME_MAX_TIME_STR_LEN 8

#define NUM_MONTHS_IN_YEAR 12

/*
 * Maximums for 24 hour clock.
 * Just for the units, specific logic is in validateTime.
 */
#define MAX_HOURS 24
#define MAX_MINUTES 59

#define MINUTES_IN_HOURS 60

/*
 * Strings for the singular, and plurals for hours and minutes.
 */
#define HOUR_DESC_SINGULAR "hour"
#define HOUR_DESC_PLURAL "hours"
#define MINUTE_DESC_SINGULAR "minute"
#define MINUTE_DESC_PLURAL "minutes"

/*
 * Format for the formatted string time.
 */
#define FORMATTED_TIME_STRING_HOURS_ONLY "%d%s"
#define FORMATTED_TIME_STRING_HOURS_MINS "%d:%02d%s"

/*
 * Pre-processor macros for the format of the duration string.
 *
 * The C compiler will auto append any strings that are placed
 * together with just whitespace seperating.
 */
#define DURATION_FORMATTED_STRING_SEG "%d %s"
#define SINGLE_UNIT_DURATION_STRING "(" DURATION_FORMATTED_STRING_SEG ")"
#define TWO_UNIT_DURATION_STRING "(" DURATION_FORMATTED_STRING_SEG ", " \
  DURATION_FORMATTED_STRING_SEG ")"

/*
 * Forward declarations.
 *
 * These are all internal functions to break down the logic into
 * easier to understand steps. (Because I'm just not that smart).
 */
static enum DateTimeError checkStrLength(const char *const stDate,
    const size_t min,
    const size_t max);
static enum DateTimeError parseDateString(const char *const stDate,
    struct Date *date);
static enum DateTimeError validateDate(int year, int month, int day);
static Boolean isLeapYear(int year);
static enum DateTimeError checkYear(int year);
static enum DateTimeError checkMonth(int month);
static enum DateTimeError checkDay(int year, int month, int day);
static enum DateTimeError parseTimeString(const char *const stTime,
    struct Time *time);
static enum DateTimeError validateTime(int hour, int minutes);

/*
 * Both dateParse and timeParse just do the simple length check
 * validation for time and date. They call the more complex
 * verification functions parseDateString and parseTimeString.
 */
enum DateTimeError dateParse(const char *const stDate, struct Date *date)
{
  enum DateTimeError error_result;
  error_result = checkStrLength(stDate, DATETIME_MIN_DATE_STR_LEN,
                                DATETIME_MAX_DATE_STR_LEN);

  if (error_result == DATETIME_NO_ERROR) {
    error_result = parseDateString(stDate, date);
  }

  return error_result;
}

/*
 * Check description of dateParse.
 */
enum DateTimeError timeParse(const char *const stTime, struct Time *time)
{
  enum DateTimeError error_result;
  error_result = checkStrLength(stTime, DATETIME_MIN_TIME_STR_LEN,
                                DATETIME_MAX_TIME_STR_LEN);

  if (error_result == DATETIME_NO_ERROR) {
    error_result = parseTimeString(stTime, time);
  }

  return error_result;
}

/*
 * Returns the formatted date for displaying to the calendar.
 */
void dateString(char *const outString, const struct Date *const date)
{
  const char *const months[] = {"January", "February", "March", "April", "May", "June",
                                "July", "August", "September", "October", "November",
                                "December"};

  sprintf(outString, "%d %s %d", date->day, months[date->month - 1],
          date->year);
}

/*
 * There is some confusion over 12:00am vs 12:00pm. As per the Wiki
 *  article:
 *
 *  http://en.wikipedia.org/wiki/12-hour_clock#Confusion_at_noon_and_midnight
 *
 * Will just go with 00:00 = 12:00am and 12:00 = 12:00pm.
 */
void timeString(char *const outString, const struct Time *const time)
{
  int hour_12; /* Hour value for 12 hour based clock. */
  const char *meridies; /* am/pm */

  if (time->hour > 11) {
    /* 12:00 - 24:00 */
    meridies = "pm";
  } else {
    /* 00:00 - 11:59 */
    meridies = "am";
  }

  hour_12 = (time->hour % 12);

  /* Special case for 0:00, because modulo will not give correct hour. */
  if (hour_12 == 0) {
    hour_12 = 12;
  }

  /* On the hour times are displayed without an minutes. */
  if (time->minutes == 0) {
    sprintf(outString, FORMATTED_TIME_STRING_HOURS_ONLY, hour_12,
            meridies);
  } else {
    sprintf(outString, FORMATTED_TIME_STRING_HOURS_MINS, hour_12,
            time->minutes, meridies);
  }
}

/*
 * Output the formatted duration to the given string.
 *
 * WARNING: This does not enforce the length of the output string.
 */
void durationString(char *const outString, int duration)
{
  int hours, minutes;
  char *hour_description, *minute_description;

  hours = duration / MINUTES_IN_HOURS;
  minutes = duration % MINUTES_IN_HOURS;

  if (hours > 1) {
    hour_description = HOUR_DESC_PLURAL;
  } else {
    hour_description = HOUR_DESC_SINGULAR;
  }

  if (minutes > 1) {
    minute_description = MINUTE_DESC_PLURAL;
  } else {
    minute_description = MINUTE_DESC_SINGULAR;
  }

  /*
   * Check that either we have only minutes, or only hours.
   */
  if ((hours != 0) != (minutes != 0)) {
    /* We have just to output hours or minutes, not both */
    int single_time_unit;
    const char *single_time_unit_desc;

    if (hours != 0) {
      single_time_unit = hours;
      single_time_unit_desc = hour_description;
    } else {
      single_time_unit = minutes;
      single_time_unit_desc = minute_description;
    }

    sprintf(outString, SINGLE_UNIT_DURATION_STRING, single_time_unit,
            single_time_unit_desc);
  } else {
    /* We have both */
    sprintf(outString, TWO_UNIT_DURATION_STRING, hours, hour_description,
            minutes, minute_description);
  }
}

/*
 * Checks the length of the date string, and will return an error code
 * if it's too big or small.
 */
static enum DateTimeError checkStrLength(const char *const stDate,
    const size_t min,
    const size_t max)
{
  enum DateTimeError error_result;
  size_t date_str_len;

  error_result = DATETIME_NO_ERROR;
  date_str_len = strnlen(stDate, max);

  if (date_str_len != max) {
    if (date_str_len < min) {
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
static enum DateTimeError parseDateString(const char *const stDate,
    struct Date *date)
{
  enum DateTimeError error_result;
  int day, month, year, scan_result;

  date->year = 0;
  date->month = 0;
  date->day = 0;

  scan_result = sscanf(stDate, FILE_DATE_FORMAT,
                       &year, &month, &day);

  /* Haven't scanned three integers, then it's a problem. */
  if (scan_result != 3 || scan_result == EOF) {
    error_result = DATETIME_INVALID;
  } else {
    error_result = validateDate(year, month, day);

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
static enum DateTimeError validateDate(int year, int month, int day)
{
  enum DateTimeError result;

  result = checkYear(year);

  if (result == DATETIME_NO_ERROR) {
    result = checkMonth(month);

    if (result == DATETIME_NO_ERROR) {
      result = checkDay(year, month, day);
    }
  }

  return result;
}

/*
 * Return true if the given year is a leap year.
 */
static Boolean isLeapYear(int year)
{
  Boolean result = FALSE;

  /* Follows the typical leap year calculation */
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
 * There is no year 0.
 */
static enum DateTimeError checkYear(int year)
{
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
static enum DateTimeError checkMonth(int month)
{
  enum DateTimeError result;
  result = DATETIME_NO_ERROR;

  if ((month < 1) || (month > NUM_MONTHS_IN_YEAR)) {
    result = DATETIME_MONTH_INVALID;
  }

  return result;
}

/*
 * February is the weird one, number of days varies by leap year.
 *
 * It is assumed that month verification has been done.
 */
static enum DateTimeError checkDay(int year, int month, int day)
{
  /*
   * 30 days hath September...
   * February has 28, unless it's a leap year.
   */
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int max_days;
  enum DateTimeError result;
  const int FEBURARY = 2;

  result = DATETIME_NO_ERROR;
  max_days = days_in_month[month - 1];

  if (isLeapYear(year) && (month == FEBURARY)) {
    max_days = 29;
  }

  if (max_days < day) {
    result = DATETIME_DAY_INVALID;
  }

  return result;
}

/*
 * Given a string, update the given time struct.
 *
 * Returns a DATETIME_INVALID error, if there's a problem.
 * DATE_NO_ERROR otherwise.
 *
 * stTime - Time string to parse, expected to be in FILE_TIME_FORMAT.
 * time - Pointer to time struct to update.
 */
static enum DateTimeError parseTimeString(const char *const stTime,
    struct Time *time)
{
  enum DateTimeError error_result;
  int hour, minutes, scan_result;

  time->hour = 0;
  time->minutes = 0;

  scan_result = sscanf(stTime, FILE_TIME_FORMAT, &hour, &minutes);

  /* Haven't scanned two integers. */
  if (scan_result != 2 || scan_result == EOF) {
    error_result = DATETIME_INVALID;
  } else {
    error_result = validateTime(hour, minutes);

    if (error_result == DATETIME_NO_ERROR) {
      time->hour = hour;
      time->minutes = minutes;
    }
  }

  return error_result;
}

/*
 * Validate Time
 *
 * Checks that minutes is never greater than 59, hours are from 0 -
 * 24, and that the 24th hour has 0 minutes.
 */
static enum DateTimeError validateTime(int hour, int minutes)
{
  enum DateTimeError result;

  result = DATETIME_NO_ERROR;

  if ((hour > MAX_HOURS) || (hour < 0)) {
    result = DATETIME_HOUR_INVALID;
  } else if ((minutes > MAX_MINUTES) || (minutes < 0)) {
    result = DATETIME_MINUTES_INVALID;
  } else if ((hour == MAX_HOURS) && (minutes > 0)) {
    result = DATETIME_MINUTES_INVALID;
  }

  return result;
}
