/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Implementation of reading from, and writing to calendar files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calendar_file.h"
#include "event_list.h"
#include "event.h"

/*
 * Name and location are variable length strings. BUFFER_CHUNK is the
 * initial size of the buffer we use to load them from the calendar
 * file.
 */
#define BUFFER_CHUNK 512

/*
 * Max string lengths to support reading the date and time strings
 * from the calendar file. These are including the terminator.
 *
 * EVENT_LEADING_FORMAT is the fscanf string for reading in these
 * strings, these should be one less than the max string lengths.
 */
#define EVENT_MAX_DATE_STR_LEN 11
#define EVENT_MAX_TIME_STR_LEN 6
#define EVENT_LEADING_FORMAT "%10s %5s %i "

/*
 * Module Ident
 *
 * Just a string used to identify this particular module, for
 * reporting errors.
 */
#define MODULE_IDENT "Calendar File: "

/*
 * Our struct for holding all the information about the current calendar file
 * we're loading.
 *
 * We need to handle variable length strings, so we maintain a read
 * buffer which will grow as required. It is handled in read_buffer,
 * and buffer_size.
 *
 * We also keep track of any errors creating an event here, this is
 * because we need to stop processing if we get a file error, or an
 * error creating an event.
 */
struct CalendarFile {
  char *read_buffer;
  int buffer_size;
  FILE *current_file;
  enum EventError event_error;
};

/*
 * Forward declarations.
 */
static enum FileError readEventFromFile(struct Event **loaded_event,
                                        struct CalendarFile *calendar_file);
static enum FileError readEventName(char **temp_name,
                                    struct CalendarFile *calendar_file);
static enum FileError readVariableLengthString(struct CalendarFile
    *calendar_file);
static enum FileError readEventLocation(char **temp_location,
                                        struct CalendarFile *calendar_file);

/*
 * Load the given calendar file into the list.
 */
enum FileError loadCalendar(struct EventList *list,
                            const char *filename)
{
  enum FileError file_error_result;
  struct CalendarFile calendar_file;

  /*
   * Check for NULL filename, empty strings will return FILE_ERROR
   * error.
   */
  if (filename != NULL) {
    calendar_file.buffer_size = 0;
    calendar_file.current_file = fopen(filename, "rb");
    calendar_file.event_error = EVENT_NO_ERROR;

    if (calendar_file.current_file != NULL) {
      /* Manged to open the file. */
      struct Event *current_event;
      Boolean event_insert_success;

      current_event = NULL;
      event_insert_success = FALSE;

      /*
       * Allocate our starting buffer for reading variable length
       * strings.
       *
       * It starts off as BUFFER_CHUNK size, but will double in size
       * each time it's not big enough.
       */
      calendar_file.read_buffer = (char *) malloc(BUFFER_CHUNK);

      if (calendar_file.read_buffer != NULL) {
        /* We have our initial read buffer. */
        calendar_file.buffer_size = BUFFER_CHUNK;

        /*
         * Keep reading from the file, until we hit the end of the
         * file, or get an error from the file read, creating the
         * event, or inserting the event into the list.
         */
        do {
          /*
           * Errors with validating the event itself are in the calendar_file struct.
           */
          file_error_result = readEventFromFile(&current_event,
                                                &calendar_file);

          /* No errors reading the event, try adding it to the list. */
          if (calendar_file.event_error == EVENT_NO_ERROR) {
            event_insert_success = eventListInsertLast(list, current_event);
          }
        } while ((calendar_file.event_error == EVENT_NO_ERROR) &&
                 (file_error_result == FILE_NO_ERROR) &&
                 event_insert_success);

        /*
         * Check to see if we got an EOF, if we have, then all is
         * right with the world, remap the code to FILE_NO_ERROR.
         */
        if (file_error_result == FILE_EOF) {
          file_error_result = FILE_NO_ERROR;
        }

        /*
         * If there was an error with inserting the last event.
         * Free the memory.
         */
        if (!event_insert_success) {
          eventDestroy(current_event);
        }

        /* Clean up our reading buffer */
        free(calendar_file.read_buffer);
      } else {
        /*
         * Couldn't allocate the memory for the read buffer, this
         * isn't recoverable.
         */
        file_error_result = FILE_INTERNAL_ERROR;
      }

      fclose(calendar_file.current_file);
    } else {
      /* File open did not succeed. */
      file_error_result = FILE_ERROR;
    }
  } else {
    file_error_result = FILE_NO_FILENAME;
  }

  return file_error_result;
}

/*
 * Save the calendar events a the given filename.
 */
enum FileError saveCalendar(struct EventList *list,
                            const char *filename)
{
  enum FileError file_error_result;
  FILE *output_file;
  struct Event *current_event;

  if (list->head != NULL) {
    if (filename != NULL) {
      file_error_result = FILE_NO_ERROR;

      output_file = fopen(filename, "wb");

      if (output_file != NULL) {
        eventListResetPosition(list);
        current_event = eventListNext(list);

        while (current_event != NULL) {
          fprintf(output_file, "%04d-%02d-%02d %02d:%02d %d %s\n",
                  current_event->date.year,
                  current_event->date.month,
                  current_event->date.day,
                  current_event->time.hour,
                  current_event->time.minutes,
                  current_event->duration,
                  current_event->name);

          if (current_event->location != NULL) {
            fprintf(output_file, "%s\n", current_event->location);
          }

          fprintf(output_file, "\n");

          current_event = eventListNext(list);
        }
      } else {
        file_error_result = FILE_ERROR;
      }
    } else {
      file_error_result = FILE_NO_FILENAME;
    }
  } else {
    file_error_result = FILE_EMPTY_LIST;
  }

  return file_error_result;
}

/*
 * Takes a file error, and returns a string that represents the text
 * of that error.
 */
char *errorString(enum FileError file_error)
{
  char *error_text;

  switch (file_error) {
  case FILE_NO_ERROR:
    error_text = MODULE_IDENT "No error.";
    break;
  case FILE_EOF:
    error_text = MODULE_IDENT "EOF reached.";
    break;
  case FILE_ERROR:
    error_text = MODULE_IDENT "Unable to read/write file.";
    break;
  case FILE_INVALID_FORMAT:
    error_text = MODULE_IDENT "File has invalid format.";
    break;
  case FILE_NO_FILENAME:
    error_text = MODULE_IDENT "Blank filename provided.";
    break;
  case FILE_INTERNAL_ERROR:
    error_text = MODULE_IDENT "Internal error, trying to recover.";
    break;
  case FILE_EMPTY_LIST:
    error_text = MODULE_IDENT "Attempted to save empty list.";
    break;
  default:
    error_text = MODULE_IDENT
                 "This is really bad, you've invented an error I don't know!";
    break;
  }

  return error_text;
}

/*
 * Tries to read an event from the current file position.
 *
 * This will allocate the memory for the event, it will also grow the
 * read buffer as needed.
 *
 * IF this function returns FILE_NO_ERROR, or FILE_EOF, then these are
 * not errors. Any formatting errors of the file will be returned as
 * FILE_INVALID_FORMAT.
 *
 * loaded_event - This is updated to point to the event read from the
 *                file. If there is an error, it will just be NULL.
 *
 * calendar_file - The CalendarFile struct, the read buffer will be
 *                 expanded in size if needed. Also, any errors with
 *                 parsing the event from the file will be returned
 *                 here.
 */
static enum FileError readEventFromFile(struct Event **loaded_event,
                                        struct CalendarFile *calendar_file)
{
  enum FileError file_error_result;
  char date[EVENT_MAX_DATE_STR_LEN], time[EVENT_MAX_TIME_STR_LEN];
  int duration, read_result;

  calendar_file->event_error = EVENT_READ_ERROR;

  /*
   * Scan in the first line of the event.
   */
  read_result = fscanf(calendar_file->current_file, EVENT_LEADING_FORMAT,
                       date, time, &duration);

  /*
   * If the file read was not exactly what we expect, then it's a file
   * format error.
   */
  if (read_result == EVENT_LEADING_FORMAT_QTY) {
    char *temp_name, *temp_location;

    temp_name = NULL;
    temp_location = NULL;

    /*
     * Read in the event name string, variable length. The read buffer
     * will be expanded as necessary, up until it hits a newline.
     * Names must end with a newline, otherwise it's an invalid file
     * format.
     */
    file_error_result = readEventName(&temp_name, calendar_file);

    if (file_error_result == FILE_NO_ERROR) {
      file_error_result = readEventLocation(&temp_location, calendar_file);
    }

    if (file_error_result == FILE_EOF || file_error_result == FILE_NO_ERROR) {
      /*
       * We might have hit the end of the file, but we still have a
       * valid event name, so we might still have a valid event.
       */
      calendar_file->event_error = eventCreate(loaded_event, date, time, duration,
                                   temp_name, temp_location);
    }

    if (temp_location != NULL) {
      free(temp_location);
    }

    if (temp_name != NULL) {
      free(temp_name);
    }

  } else {
    /*
     * Didn't get the start of an event when we expected one.
     * If we get anm EOF here, it probably means that we've got no more entries.
     */
    if (feof(calendar_file->current_file)) {
      file_error_result = FILE_EOF;
    } else {
      file_error_result = FILE_INVALID_FORMAT;
    }
  }

  return file_error_result;
}

/*
 * Attempts to read the event name from the current file position.
 *
 * temp_name - Returns a character pointer to the name string of the event.
 * calendar_file - Calendar file struct for holding our read buffer, etc.
 *
 */
static enum FileError readEventName(char **temp_name,
                                    struct CalendarFile *calendar_file)
{
  enum FileError file_error_result;
  size_t name_string_length;

  file_error_result = readVariableLengthString(calendar_file);

  /*
   * Reading the EOF is fine, as long as we have a valid name string
   * still.
   */
  if ((file_error_result == FILE_NO_ERROR) ||
      (file_error_result == FILE_EOF)) {

    name_string_length = strnlen(calendar_file->read_buffer,
                                 calendar_file->buffer_size);

    /*
     * If we don't have a certain number of characters for the name,
     * file format is invalid.
     */
    if (name_string_length < EVENT_NAME_MIN_LENGTH) {
      file_error_result = FILE_INVALID_FORMAT;
    } else {
      /*
       * Allocate a temporary string to hold the name until we can create an event.
       */
      *temp_name = (char *) malloc(name_string_length + 1);

      if (*temp_name != NULL) {
        **temp_name = '\0';

        strncat(*temp_name, calendar_file->read_buffer, name_string_length);
      } else {
        file_error_result = FILE_INTERNAL_ERROR;
      }
    }
  }

  return file_error_result;
}

/*
 * Attempts to read the event location from the current file position.
 *
 * temp_location - Returns a character pointer to the location string
 *                 of the event.
 * calendar_file - Calendar file struct for holding our read buffer,
 *                 etc.
 */
static enum FileError readEventLocation(char **temp_location,
                                        struct CalendarFile *calendar_file)
{
  enum FileError file_error_result;
  size_t location_string_length;

  file_error_result = readVariableLengthString(calendar_file);

  if (file_error_result == FILE_NO_ERROR) {
    location_string_length = strnlen(calendar_file->read_buffer,
                                     calendar_file->buffer_size);

    if (location_string_length > 1) {
      *temp_location = (char *) malloc(location_string_length + 1);

      if (*temp_location != NULL) {
        **temp_location = '\0';

        strncat(*temp_location, calendar_file->read_buffer, location_string_length);
      } else {
        file_error_result = FILE_INTERNAL_ERROR;
      }
    }
  }

  return file_error_result;
}

/*
 * From the current position of the file, read in a variable length string.
 * up to a newline character, or end of file.
 *
 * We will get a needless expansion of the read buffer if we hit the
 * EOF without getting a newline character before it.
 *
 * An EOF here is not an error, but has to be returned as a separate
 * code so the caller knows to stop reading the file.
 */
static enum FileError readVariableLengthString(struct CalendarFile
    *calendar_file)
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

  while (not_upto_eol) {
    read_result = fgets(current_buffer_position, buffer_space_remaining,
                        calendar_file->current_file);

    if (read_result != NULL) {

      length = strnlen(calendar_file->read_buffer,
                       calendar_file->buffer_size - 1);

      /* We read until the end of the line? */
      if (calendar_file->read_buffer[length - 1] == '\n') {
        calendar_file->read_buffer[length - 1] = '\0';
        not_upto_eol = FALSE;
      } else {
        /* Reallocate */
        calendar_file->read_buffer = (char *) realloc(calendar_file->read_buffer,
                                     calendar_file->buffer_size * 2);
        /* Total size remaining (including space used by terminator) */
        buffer_space_remaining = calendar_file->buffer_size + 1;
        current_buffer_position = calendar_file->read_buffer +
                                  calendar_file->buffer_size - 1;
        calendar_file->buffer_size *= 2;
      }
    } else {
      /* EOF or Error */
      if (feof(calendar_file->current_file)) {
        error_result = FILE_EOF;
        not_upto_eol = FALSE;
      } else {
        error_result = FILE_ERROR;
      }

      /* Be sure to terminate the buffer string */
      *current_buffer_position = '\0';
    }
  }

  return error_result;
}
