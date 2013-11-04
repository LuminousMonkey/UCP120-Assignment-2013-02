/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * Event handling functions. The only way that events can be touched
 * is through these.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "event.h"

/*
 * Forward declarations.
 */
static int eventString(struct Event *const event, char **const outString);
static Boolean durationValid(int duration);
enum EventError eventSetName(const char *const name, char **event_name);
enum EventError eventSetLocation(const char *const name,
                                 char **location);
void eventDestroy(struct Event *event);

/*
 * Creates an event.
 *
 * Does not everify new_even is not NULL.
 * It is expected that date and time are passed in as strings.
 * The date/time code is supposed to handle parsing those.
 */
enum EventError eventCreate(struct Event **new_event,
                            const char *const stDate,
                            const char *const stTime,
                            int duration,
                            const char *const name,
                            const char *const location)
{
  enum EventError error_result;
  error_result = EVENT_NO_ERROR;

  *new_event = (struct Event *) malloc(sizeof(struct Event));

  if (*new_event != NULL) {
    (*new_event)->name = NULL;
    (*new_event)->location = NULL;
    (*new_event)->formatted_string = NULL;

    if (dateParse(stDate, &(*new_event)->date) == DATETIME_NO_ERROR) {
      if (timeParse(stTime, &(*new_event)->time) == DATETIME_NO_ERROR) {
        if (durationValid(duration)) {
          (*new_event)->duration = duration;

          if (name != NULL && name[0] != '\0') {
            eventSetName(name, &(*new_event)->name);
            eventSetLocation(location, & (*new_event)->location);

            (*new_event)->formatted_string_length = eventString(*new_event,
                                                    &(*new_event)->formatted_string);
          } else {
            error_result = EVENT_NAME_INVALID;
          }
        } else {
          error_result = EVENT_DURATION_INVALID;
        }
      } else {
        error_result = EVENT_TIME_INVALID;
      }
    } else {
      error_result = EVENT_DATE_INVALID;
    }

    /* Clean up if any errors occured */
    if (error_result != EVENT_NO_ERROR) {
      eventDestroy(*new_event);
      *new_event = NULL;
    }
  } else {
    error_result = EVENT_INTERNAL_ERROR;
  }

  return error_result;
}

/*
 * Produces a string that represets the event as per the assignment
 * spec. Takes a pointer to an event and a char string pointer.
 * Allocates the memory for the event string. Also returns an int that
 * is the size of the string (excluding the terminator.)
 *
 * This has to be called everytime an event is created or edited.
 *
 * It's doubling up storage space, but it makes building up the string
 * for all the events a bit easier.
 */
static int eventString(struct Event *const event, char **const outString)
{
  int string_length;

  char date_string[MAX_DATE_STRING];
  char time_string[MAX_TIME_STRING];
  char duration_string[MAX_DURATION_STRING];

  string_length = strlen(event->name) + 1;

  if (event->location != NULL) {
    string_length += strlen(event->location) + 3;
  }

  durationString(duration_string, event->duration);
  string_length += strnlen(duration_string, MAX_DURATION_STRING) + 1;

  dateString(date_string, &event->date);
  string_length += strnlen(date_string, MAX_DATE_STRING) + 2;

  timeString(time_string, &event->time);
  string_length += strnlen(time_string, MAX_TIME_STRING);

  /*
   * Allocate the string (+1 for terminator, returned string size does
   * not include this.)
   */
  *outString = (char *)malloc(string_length + 1);

  /* Can't recover from a memory error like this. */
  assert(outString != NULL);

  /* String building here */
  **outString = '\0';
  strcat(*outString, event->name);
  strcat(*outString, " ");

  if (event->location != NULL) {
    /* Add the location string */
    strcat(*outString, "@ ");
    strcat(*outString, event->location);
    strcat(*outString, " ");
  }

  strncat(*outString, duration_string, MAX_DURATION_STRING);
  strcat(*outString, "\n");
  strncat(*outString, date_string, MAX_DATE_STRING);

  strcat(*outString, ", ");
  strncat(*outString, time_string, MAX_TIME_STRING);

  return string_length;
}

enum EventError eventSetName(const char *const name, char **event_name)
{
  enum EventError result;
  size_t name_length;

  result = EVENT_NO_ERROR;
  name_length = 0;

  if (name != NULL) {
    name_length = strnlen(name, MAX_LENGTH_OF_NAME);
  }

  if (name_length > 0) {
    *event_name = (char *) malloc(name_length + 1);

    if (*event_name != NULL) {
      **event_name = '\0';
      strncat(*event_name, name, name_length);
    } else {
      result = EVENT_INTERNAL_ERROR;
    }
  } else {
    result = EVENT_NAME_INVALID;
  }

  return result;
}

enum EventError eventSetLocation(const char *const location,
                                 char **event_location)
{
  enum EventError result;
  size_t location_length;

  result = EVENT_NO_ERROR;
  location_length = 0;
  *event_location = NULL;

  if (location != NULL) {
    location_length = strnlen(location, MAX_LENGTH_OF_LOCATION);
  }

  if (location_length > 0) {
    *event_location = (char *) malloc(location_length + 1);

    if (*event_location != NULL) {
      **event_location = '\0';
      strncat(*event_location, location, location_length);
    } else {
      result = EVENT_INTERNAL_ERROR;
    }
  }

  return result;
}

/*
 * Destroys an event.
 *
 * Frees up the whole event, including any allocated strings for name
 * and location.
 */
void eventDestroy(struct Event *event)
{
  if (event != NULL) {
    if (event->name != NULL) {
      free(event->name);
    }

    if (event->location != NULL) {
      free(event->location);
    }

    if (event->formatted_string != NULL) {
      free(event->formatted_string);
    }

    free(event);
  }
}

/*
 * Durations must be a non-negative integer.
 */
static Boolean durationValid(int duration)
{
  return (duration >= 0);
}
