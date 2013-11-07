/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * All the date/time verification and formatting functions.
 */

#ifndef DATE_TIME_H_
#define DATE_TIME_H_

/* Max length of buffer for output of date strings */
#define MAX_DATE_STRING 20
#define MAX_TIME_STRING 8
#define MAX_DURATION_STRING 45

/*
 * How we expect the date to be formatted in the calendar file. Order
 * is important, if the format changes the order of year, month, day,
 * then parseDateString needs to be updated.
 */
#define FILE_DATE_FORMAT "%04d-%02d-%02d"

/*
 * How we expect the 24 hour time to be formatted in the calendar
 * file.
 */
#define FILE_TIME_FORMAT "%02d:%02d"

/*
 * Date structure.
 *
 * day - day of the time (1 to 31)
 * month - month (1 to 12)
 * year - year (0 to max signed int)
 */
struct Date {
  int day;
  int month;
  int year;
};

/*
 * Time structure.
 *
 * hour - hour in 24 hour based clock.
 * minutes - minutes (0 to 59)
 */
struct Time {
  int hour;
  int minutes;
};

/*
 * Our return values for possible errors.
 */
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

/*
 * Parse the given string, placing the result in the given date struct.
 * The date struct will be set to 0-0-0 on an error.
 *
 * Returns: Error about what was invalid, DATETIME_NO_ERROR if no
 *          error.
 * stDate - String that needs to be parsed.
 * date - Struct that is to be updated.
 */
enum DateTimeError dateParse(const char *const stDate, struct Date *date);

/*
 * Given a string that represents the time, update the given struct
 * with that time. If the date string is not valid, then they time
 * struct will be set to 0-0-0;
 *
 * stTime - String that needs to be parsed.
 * time - Struct that is to be updated.
 */
enum DateTimeError timeParse(const char *const stTime, struct Time *time);

/*
 * Format the given date to a string.
 *
 * The date is already assumed to be validated.
 *
 * outString - String that will be updated with formatted date, must
 *             be at least MAX_DATE_STRING long.
 * date - Date struct that has the date to format.
 */
void dateString(char *const outString, const struct Date *const date);

/*
 * Format the given time to a string.
 *
 * Time is assumed to be validated.
 *
 * outString - String that will be updated with formatted time, must
 *             be at least MAX_TIME_STRING long.
 * time - Time struct that has the time to be formatted.
 */
void timeString(char *const outString, const struct Time *const time);

/*
 * Given a duration in minutes, update the given string to have the
 * duration formatted as per the assignment spec.
 *
 * Duration is expected to have been validated.
 *
 * outString - String that will be updated with formatted duration,
 *             must be at least MAX_DURATION_STRING long.
 * duration - Duration in minutes to be formatted.
 */
void durationString(char *const outString, int duration);

#endif
