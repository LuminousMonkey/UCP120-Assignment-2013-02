/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * Calendar events.
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <stdlib.h>

#include "date_time.h"

/*
 * Number of fields expected to be read from leading format of event
 * line.
 */
#define EVENT_LEADING_FORMAT_QTY 3

/*
 * We must have at least this number of characters to consider the
 * name a valid length.
 */
#define EVENT_NAME_MIN_LENGTH 3

/*
 * Max length for name and location strings, and buffers must be this
 * size plus 1.
 */
#define MAX_LENGTH_OF_NAME 1024
#define MAX_LENGTH_OF_LOCATION 1024

/*
 * Struct for containing the event.
 */
struct Event {
  struct Date date;
  struct Time time;
  int duration;
  char *name;
  char *location; /* Set to null if no location. */
  char *formatted_string;
  int formatted_string_length;
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
  EVENT_READ_ERROR,
  EVENT_INTERNAL_ERROR
};

/*
 * Functions
 */

/*
 * Create the event.
 */
enum EventError eventCreate(struct Event **new_event,
                            const char *const stDate,
                            const char *const stTime,
                            int duration,
                            const char *const name,
                            const char *const location);

void eventDestroy(struct Event *event);

enum EventError eventSetName(const char *const name, char **event_name);
enum EventError eventSetLocation(const char *const name, char **event_name);

#endif
