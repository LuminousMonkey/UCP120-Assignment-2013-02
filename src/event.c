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
static void updateEventString(struct Event *const event);
static Boolean durationValid(int duration);
static enum EventError eventSetName(struct Event *const event,
                                    const char *const name);
static enum EventError eventSetLocation(struct Event *const event,
                                        const char *const location);
void eventDestroy(struct Event *const event);

/*
 * Creates an event.
 *
 * Note: Only returns the first error it comes across. Probably need
 * to update so that it will verify all fields so the user can know
 * which ones are faulty.
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

  assert(new_event != NULL);

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
            eventSetName(*new_event, name);
            eventSetLocation(*new_event, location);

            updateEventString(*new_event);

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
 * Edit the given event.
 *
 * Like event create, only returns the first error it's found.
 */
enum EventError eventEdit(struct Event *event_to_edit,
                          const char *const stDate,
                          const char *const stTime,
                          const int duration,
                          const char *const name,
                          const char *const location)
{
  enum EventError error_result;
  struct Event *temp_event;

  /* Temp event
   *
   * We create this event, and if it's successful, we copy the values
   * across to the event that's been passed in.
  */

  error_result = eventCreate(&temp_event, stDate, stTime, duration, name,
                             location);

  if (error_result == EVENT_NO_ERROR) {
    /*
     * No error creating the event, so it validates, update the old
     * event to match.
     */
    eventSetName(event_to_edit, temp_event->name);
    eventSetLocation(event_to_edit, temp_event->location);
    event_to_edit->date = temp_event->date;
    event_to_edit->time = temp_event->time;
    event_to_edit->duration = temp_event->duration;

    /*
     * Copying the string from the temp event would be faster, but
     * want to make sure mistake here show up quickly in the
     * textbox.
     */
    updateEventString(event_to_edit);
    event_to_edit->formatted_string_length =
      strlen(event_to_edit->formatted_string);
  }
  eventDestroy(temp_event);

  return error_result;
}

/*
 * Destroys an event.
 *
 * Frees up the whole event, including any allocated strings for name
 * and location.
 */
void eventDestroy(struct Event *const event)
{
  if (event != NULL) {
    free(event->name);
    free(event->location);
    free(event->formatted_string);
    free(event);
  }
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
 *
 * event - Event to build the formatted string for.
 */
static void updateEventString(struct Event *const event)
{
  int string_length;

  char date_string[MAX_DATE_STRING];
  char time_string[MAX_TIME_STRING];
  char duration_string[MAX_DURATION_STRING];

  free(event->formatted_string);

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
  event->formatted_string = (char *)malloc(string_length + 1);

  /* Can't recover from a memory error like this. */
  assert(event->formatted_string != NULL);

  /* String building here */
  *event->formatted_string = '\0';
  strcat(event->formatted_string, event->name);
  strcat(event->formatted_string, " ");

  if (event->location != NULL) {
    /* Add the location string */
    strcat(event->formatted_string, "@ ");
    strcat(event->formatted_string, event->location);
    strcat(event->formatted_string, " ");
  }

  strncat(event->formatted_string, duration_string, MAX_DURATION_STRING);
  strcat(event->formatted_string, "\n");
  strncat(event->formatted_string, date_string, MAX_DATE_STRING);

  strcat(event->formatted_string, ", ");
  strncat(event->formatted_string, time_string, MAX_TIME_STRING);

  event->formatted_string_length = string_length;
}

/*
 * Copies over the name string into the given event.
 *
 * event - Event that we want to update.
 * name - String that the event's name should be updated to.
 */
static enum EventError eventSetName(struct Event *const event,
                                    const char *const name)
{
  enum EventError result;
  size_t name_length;

  result = EVENT_NO_ERROR;
  name_length = 0;

  assert(event != NULL);

  if (name != NULL) {
    name_length = strnlen(name, MAX_LENGTH_OF_NAME);
  }

  if (name_length > 0) {
    /* Free up memory already allocated to the existing event name. */
    free(event->name);
    event->name = (char *) malloc(name_length + 1);

    if (event->name != NULL) {
      *event->name = '\0';
      strncat(event->name, name, name_length);
    } else {
      result = EVENT_INTERNAL_ERROR;
    }
  } else {
    result = EVENT_NAME_INVALID;
  }

  return result;
}

/*
 * Copies over the location string into the given event.
 *
 * event - Event that we want to update.
 * location - String that the event's location should be updated to.
 */
static enum EventError eventSetLocation(struct Event *const event,
                                        const char *const location)
{
  enum EventError result;
  size_t location_length;

  result = EVENT_NO_ERROR;
  location_length = 0;

  assert(event);

  /* Location is allowed to be NULL, so free the memory now. */
  free(event->location);
  event->location = NULL;

  if (location != NULL) {
    location_length = strnlen(location, MAX_LENGTH_OF_LOCATION);
  }

  if (location_length > 0) {
    event->location = (char *) malloc(location_length + 1);

    if (event->location != NULL) {
      *event->location = '\0';
      strncat(event->location, location, location_length);
    } else {
      /* Couldn't allocate memory. */
      result = EVENT_INTERNAL_ERROR;
    }
  }

  return result;
}

/*
 * Durations must be a non-negative integer.
 *
 * Returns TRUE if the duration is valid.
 *
 * duration - Duration to validate.
 */
static Boolean durationValid(int duration)
{
  return (duration >= 0);
}
