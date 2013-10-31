/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * Event handling functions. The only way that events can be touched
 * is through these.
 */

#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "event.h"

#define MAX_LENGTH_OF_NAME 1024
#define MAX_LENGTH_OF_LOCATION 1024

/*
 * Forward declarations.
 */
static Boolean durationValid( int duration );
enum EventError eventSetName( const char *const name, char **event_name );
enum EventError eventSetLocation( const char *const name,
                                  char **location );
void eventDestroy( struct Event *event );

/*
 * Creates an event.
 *
 * Does not everify new_even is not NULL.
 * It is expected that date and time are passed in as strings.
 * The date/time code is supposed to handle parsing those.
 */
enum EventError eventCreate( struct Event **new_event,
                             const char *const stDate,
                             const char *const stTime,
                             int duration,
                             const char *const name,
                             const char *const location ) {
  enum EventError error_result;
  error_result = EVENT_NO_ERROR;

  *new_event = ( struct Event * ) malloc( sizeof( struct Event ) );

  ( *new_event )->name = NULL;
  ( *new_event )->location = NULL;

  if ( *new_event != NULL ) {
    if ( dateParse( stDate, &( *new_event )->date ) == DATETIME_NO_ERROR ) {
      if ( timeParse( stTime, &( *new_event )->time ) == DATETIME_NO_ERROR ) {
        if ( durationValid( duration ) ) {
          ( *new_event )->duration = duration;

          if ( name != NULL && name[0] != '\0' ) {
            eventSetName( name, &( *new_event )->name );
            eventSetLocation( location, & ( *new_event )->location );
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

  if ( error_result != EVENT_NO_ERROR ) {
    eventDestroy( *new_event );
    *new_event = NULL;
  }

  return error_result;
}

enum EventError eventSetName( const char *const name, char **event_name ) {
  enum EventError result;
  size_t name_length;

  result = EVENT_NO_ERROR;
  name_length = 0;

  if ( name != NULL ) {
    name_length = strnlen( name, MAX_LENGTH_OF_NAME );
  }

  if ( name_length > 0 ) {
    *event_name = ( char * ) malloc( name_length + 1 );

    if ( *event_name != NULL ) {
      **event_name = '\0';
      strncat( *event_name, name, name_length );
    } else {
      result = EVENT_INTERNAL_ERROR;
    }
  } else {
    result = EVENT_NAME_INVALID;
  }

  return result;
}

enum EventError eventSetLocation( const char *const location,
                                  char **event_location ) {
  enum EventError result;
  size_t location_length;

  result = EVENT_NO_ERROR;
  location_length = 0;
  *event_location = NULL;

  if ( location != NULL ) {
    location_length = strnlen( location, MAX_LENGTH_OF_LOCATION );
  }

  if ( location_length > 0 ) {
    *event_location = ( char * ) malloc( location_length + 1 );

    if ( *event_location != NULL ) {
      **event_location = '\0';
      strncat( *event_location, location, location_length );
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
void eventDestroy( struct Event *event ) {
  if ( event != NULL ) {
    if ( event->name != NULL ) {
      free( event->name );
    }

    if ( event->location != NULL ) {
      free( event->location );
    }

    free( event );
  }
}

/*
 * Durations must be a non-negative integer.
 */
static Boolean durationValid( int duration ) {
  return ( duration >= 0 );
}
