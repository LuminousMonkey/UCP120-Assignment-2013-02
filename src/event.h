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
 * name, and location a valid length.
 */
#define EVENT_NAME_MIN_LENGTH 3

/*
 * Max length for name and location strings, and buffers must be this
 * size plus 1.
 */
#define MAX_LENGTH_OF_NAME 1024
#define MAX_LENGTH_OF_LOCATION 1024

/*
 * The actual events.
 *
 * The strings are all dynamically allocated.
 *
 * date/time - Be sure to just use the date_time.h functions.
 *
 * formatted_string - This is a string that represents the event's
 *                    on-screen calendar display. It is automatically
 *                    updated when using the eventCreate and eventEdit
 *                    functions.
 * formatted_string_length - Maintained count of length of formatted string,
 *                           just so we don't need strlen calls when looping
 *                           to create calendar display.
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
 * Create the event.
 *
 * This should be the only way to create events. Pass in string for
 * the fields and it will verify and return the first error it comes
 * across.
 *
 * Returns an error indicating which field was faulty.
 *
 * new_event - Set to a pointer to the new event if everything was
 *             valid. NULL if there was an error.
 * stDate - Date string, formatted as defined by FILE_DATE_FORMAT.
 * stTime - Date string, formatted as defined by FILE_TIME_FORMAT.
 * duration - Duration in minutes of the event. Must not be negative.
 * name - Name of the event, up to MAX_LENGTH_OF_NAME. Error if NULL
 *        or empty.
 * location - Location of event, up to MAX_LENGTH_OF_LOCATION.
 */
enum EventError eventCreate(struct Event **new_event,
                            const char *const stDate,
                            const char *const stTime,
                            int duration,
                            const char *const name,
                            const char *const location);

/*
 * Edit an event.
 *
 * This should be the only way to edit events. Pass in string for
 * the fields and it will verify and return the first error it comes
 * across.
 *
 * The event will not be changed if there was an error with one of the
 * fields.
 *
 * Returns an error indicating which field was faulty.
 *
 * event_to_edit - Pointer to the event to edit.
 * stDate - Date string, formatted as defined by FILE_DATE_FORMAT.
 * stTime - Date string, formatted as defined by FILE_TIME_FORMAT.
 * duration - Duration in minutes of the event. Must not be negative.
 * name - Name of the event, up to MAX_LENGTH_OF_NAME. Error if NULL
 *        or empty.
 * location - Location of event, up to MAX_LENGTH_OF_LOCATION.
 */
enum EventError eventEdit(struct Event *event_to_edit,
                          const char *const stDate,
                          const char *const stTime,
                          const int duration,
                          const char *const name,
                          const char *const location);

/*
 * Destroy an event.
 *
 * Frees up all the memory allocated to an event.
 *
 * event - Pointer to the event to destroy. If NULL nothing is done.
 */
void eventDestroy(struct Event *event);

#endif
