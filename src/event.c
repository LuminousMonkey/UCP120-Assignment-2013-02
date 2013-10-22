/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * Event handling functions. The only way that events can be touched
 * is through these.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "event.h"

/*
 * Creates an event.
 *
 * It is expected that date and time are passed in as strings.
 * The date/time code is supposed to handle parsing those.
 */
enum EventError event_create(struct Event **new_event,
                             const char const *stDate,
                             const char const *stTime,
                             int duration,
                             const char const *name,
                             const char const *location) {
  enum EventError error_result;
  error_result = EVENT_NO_ERROR;

  *new_event = (struct Event *)malloc(sizeof(struct Event));

  if (*new_event != NULL) {
    // Get the date in.
    if (date_parse(stDate, &new_event->date) == DATETIME_NO_ERROR) {
      if (time_parse(stTime, &new_event->date) == DATETIME_NO_ERROR) {
        if (duration_valid(duration)) {
          (*new_event)->duration = duration;
          if (name != NULL && name[0] != '\0') {
            event_set_name(name, (*new_event)->name);
            event_set_location(location, (*new_event->location));
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
  } else {
    error_result = EVENT_INTERNAL_ERROR;
  }

  return error_result;
}

/*
 * Destroys an event.
 *
 * Frees up the whole event, including any allocated strings for name
 * and location.
 */
void event_destroy(struct Event* event) {
  if (event != NULL) {
    if (event->name != NULL) {
      free(event->name);
    }

    if (event->location != NULL) {
      free(event->location);
    }

    free(event);
  }
}

/*
 * Durations must be a non-negative integer.
 */
static Bool duration_valid(int duration) {
  return (duration >= 0);
}
