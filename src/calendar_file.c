/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calendar_file.h"
#include "event_list.h"
#include "event.h"

#define BUFFER_CHUNK 10

/*
 * Our struct for holding all the information about the current calendar file
 * we're loading.
 */
struct CalendarFile
  {
  /* Our buffer for reading in variable length strings */
  char *read_buffer;
  int buffer_size;
  FILE *current_file;
  /* Error status of last read event */
  enum EventError event_error;
  };

/*
 * Forward declarations.
 */
static enum FileError readEventFromFile( struct CalendarFile *calendar_file,
    struct Event **loaded_event );
static enum FileError readVariableLengthString( struct CalendarFile
    *calendar_file );

/*
 * Load the given calendar file into the list.
 *
 * The list must be initalised.
 */
enum FileError loadCalendar( struct EventList *list,
                             const char *filename )
  {
  enum FileError error_result;
  struct CalendarFile calendar_file;

  error_result = FILE_NO_ERROR;

  calendar_file.buffer_size = 0;
  calendar_file.current_file = fopen( filename, "rb" );

  if ( calendar_file.current_file != NULL )
    {
    struct Event *current_event;
    Boolean event_insert_success = FALSE;

    /* Allocate our starting buffer for reading variable length strings */
    calendar_file.read_buffer = ( char * ) malloc( BUFFER_CHUNK );

    if ( calendar_file.read_buffer != NULL )
      {
      calendar_file.buffer_size = BUFFER_CHUNK;

      do
        {
        error_result = readEventFromFile( &calendar_file,
                                          &current_event );

        if ( error_result == FILE_NO_ERROR )
          {
          event_insert_success = eventListInsertLast( list, current_event );
          }
        }
      while ( ( calendar_file.event_error == EVENT_NO_ERROR ) &&
              ( error_result == FILE_NO_ERROR ) &&
              event_insert_success );

      if ( !event_insert_success )
        {
        eventDestroy( current_event );
        eventListDestroy( list );
        error_result = FILE_INVALID_FORMAT;
        }

      free( calendar_file.read_buffer );
      }
    else
      {
      error_result = FILE_INTERNAL_ERROR;
      }

    fclose( calendar_file.current_file );
    }
  else
    {
    error_result = FILE_ERROR;
    }

  return error_result;
  }

enum FileError save_calendar( const struct EventList *list,
                              const char *filename );

/*
 * From the current position of the file, tries to read in an
 * event.
 */
static enum FileError readEventFromFile( struct CalendarFile *calendar_file,
    struct Event **loaded_event )
  {
  enum FileError error_result;
  char date[EVENT_MAX_DATE_STR_LEN], time[EVENT_MAX_TIME_STR_LEN];
  int duration, read_result;

  error_result = FILE_NO_ERROR;

  /*
   * Scan in the first line of the event.
   *
   * Be sure to check event.h, EVENT_MAX_DATE_STR_LEN and
   * EVENT_MAX_TIME_STR_LEN determine the values to use in this format
   * string.
   */
  read_result = fscanf( calendar_file->current_file, EVENT_LEADING_FORMAT,
                        date, time, &duration );

  if ( read_result == EVENT_LEADING_FORMAT_QTY )
    {
    char *temp_name, *temp_location;
    size_t name_string_length, location_string_length;

    temp_name = NULL;
    temp_location = NULL;

    readVariableLengthString( calendar_file );

    name_string_length = strnlen( calendar_file->read_buffer,
                                  calendar_file->buffer_size );

    temp_name = ( char * ) malloc( name_string_length + 1 );

    if ( temp_name != NULL )
      {
      *temp_name = '\0';

      strncat( temp_name, calendar_file->read_buffer, name_string_length );
      }

    readVariableLengthString( calendar_file );

    location_string_length = strnlen( calendar_file->read_buffer,
                                      calendar_file->buffer_size );

    if ( location_string_length > 0 )
      {
      temp_location = ( char * ) malloc( location_string_length + 1 );

      if ( temp_location != NULL )
        {
        *temp_location = '\0';

        strncat( temp_location, calendar_file->read_buffer, location_string_length );
        }
      }

    /* Create the event */
    calendar_file->event_error = eventCreate( loaded_event, date, time, duration,
                                 temp_name, temp_location );

    if ( temp_location != NULL )
      {
      free( temp_location );
      }

    if ( temp_name != NULL )
      {
      free( temp_name );
      }
    }
  else
    {
    /* Check it it's because we hit the end of file */
    if ( feof( calendar_file->current_file ) )
      {
      error_result = FILE_EOF;
      }
    else
      {
      error_result = FILE_INVALID_FORMAT;
      }
    }

  return error_result;
  }

/*
 * From the current position of the file, read in a variable length string.
 * up to a newline character, or end of file.
 */
static enum FileError readVariableLengthString( struct CalendarFile
    *calendar_file )
  {
  enum FileError error_result;
  size_t length;
  int buffer_space_remaining;
  Boolean not_upto_eol;
  char *current_buffer_position, *read_result;

  error_result = FILE_NO_ERROR;

  not_upto_eol = TRUE;

  current_buffer_position = calendar_file->read_buffer;
  buffer_space_remaining = calendar_file->buffer_size;

  while ( not_upto_eol )
    {
    read_result = fgets( current_buffer_position, buffer_space_remaining,
                         calendar_file->current_file );

    if ( read_result != NULL )
      {

      length = strnlen( calendar_file->read_buffer,
                        calendar_file->buffer_size - 1 );

      /* We read until the end of the line? */
      if ( calendar_file->read_buffer[length - 1] == '\n' )
        {
        calendar_file->read_buffer[length - 1] = '\0';
        not_upto_eol = FALSE;
        }
      else
        {
        /* Reallocate */
        calendar_file->read_buffer = ( char * ) realloc( calendar_file->read_buffer,
                                     calendar_file->buffer_size * 2 );
        /* Total size remaining (including space used by terminator) */
        buffer_space_remaining = calendar_file->buffer_size + 1;
        current_buffer_position = calendar_file->read_buffer +
                                  calendar_file->buffer_size - 1;
        calendar_file->buffer_size *= 2;
        }
      }
    else
      {
      /* EOF or Error */
      if ( feof( calendar_file->current_file ) )
        {
        error_result = FILE_EOF;
        }
      else
        {
        error_result = FILE_ERROR;
        }
      }
    }

  return error_result;
  }
