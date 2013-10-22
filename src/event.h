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
 * Struct for containing the event.
 */
struct Event {
  struct Date date;
  struct Time time;
  int duration;
  char *name;
  char *location; /* Set to null if no location. */
};

/*
 * For communicating the type of errors that we might get with events.
 */
enum EventError {
  EVENT_NO_ERROR,
  EVENT_DATE_INVALID,
  EVENT_TIME_INVALID,
  EVENT_DURATION_INVALID,
  EVENT_NAME_INVALID,
  EVENT_INTERNAL_ERROR
};

#endif
