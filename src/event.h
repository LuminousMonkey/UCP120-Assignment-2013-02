/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * Calendar events.
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "date_time.h"

/*
 * Max string length with terminator.
 *
 * If these are updated, update the format string for the fscanf call
 * in calendar_file.c Unfortunately the c preprocessor doesn't let you
 * stringify % easily, so we have to hardcode the string here rather
 * than build it up from the existing constants.
 */
#define EVENT_MAX_DATE_STR_LEN 11
#define EVENT_MAX_TIME_STR_LEN 6
#define EVENT_LEADING_FORMAT "%10s %5s %i "
/*
 * Number of fields expected to be read from leading format of event
 * line.
 */
#define EVENT_LEADING_FORMAT_QTY 3

/*
 * Struct for containing the event.
 */
struct Event
  {
  struct Date date;
  struct Time time;
  int duration;
  char *name;
  char *location; /* Set to null if no location. */
  };

/*
 * For communicating the type of errors that we might get with events.
 */
enum EventError
{
  EVENT_NO_ERROR,
  EVENT_DATE_INVALID,
  EVENT_TIME_INVALID,
  EVENT_DURATION_INVALID,
  EVENT_NAME_INVALID,
  EVENT_READ_ERROR,
  EVENT_INTERNAL_ERROR
};

/*
 * Functions
 */

/*
 * Create the event.
 */
enum EventError eventCreate( struct Event **new_event,
                             const char *const stDate,
                             const char *const stTime,
                             int duration,
                             const char *const name,
                             const char *const location );

void event_destroy( struct Event* event );

#endif
