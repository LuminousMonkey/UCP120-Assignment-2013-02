/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * All the date/time verification and formatting functions.
 */

#ifndef DATE_TIME_H_
#define DATE_TIME_H_

#include "bool.h"

/*
 * The max and min length we expect for date strings. If the date
 * string is DATETIME_MAX_STR_LEN or greater then it's considered too
 * long.
 */
#define DATETIME_MIN_DATE_STR_LEN 6
#define DATETIME_MAX_DATE_STR_LEN 20

#define DATETIME_MIN_TIME_STR_LEN 3
#define DATETIME_MAX_TIME_STR_LEN 8

/* Max length of buffer for output of date strings */
#define MAX_DATE_STRING 20
#define MAX_TIME_STRING 8

struct Date {
  int day;
  int month;
  int year;
};

struct Time {
  int hour;
  int minutes;
};

enum DateTimeError {
  DATETIME_NO_ERROR,
  DATETIME_STR_TOO_SHORT,
  DATETIME_STR_TOO_LONG,
  DATETIME_YEAR_INVALID,
  DATETIME_MONTH_INVALID,
  DATETIME_DAY_INVALID,
  DATETIME_HOUR_INVALID,
  DATETIME_MINUTES_INVALID,
  DATETIME_INVALID /* If the whole date is invalid. */
};

enum DateTimeError dateParse( const char *const stDate, struct Date *date );
enum DateTimeError timeParse( const char *const stTime, struct Time *time );

/*
 * Given a date, and a poiner to an already allocated char array of at
 * least size MAX_DATE_STRING length, return the formatted date.
 *
 * The date is already assumed to be validated.
 *
 */
void dateString( char *const outString, const struct Date *const date );

/*
 * Given a date, and a pointer to an already allocated char array of
 * at least size MAX_TIME_STRING length, return a formatted date.
 */
void timeString( char *const outString, const struct Time *const time );

/*
 * Given an int that represents a valid duration, return the string of
 * that duration formatted as specified in the assignment spec.
 */
void durationString(char *const outString, int duration);

#endif
