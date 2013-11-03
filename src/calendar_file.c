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
static enum FileError readVariableLengthString(struct CalendarFile
                                               *calendar_file);

/*
 * Load the given calendar file into the list.
 */
enum FileError loadCalendar(struct EventList *list,
                            const char *filename) {
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
      current_event = NULL;
      Boolean event_insert_success = FALSE;

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
          if (file_error_result == FILE_NO_ERROR) {
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
         * If we couldn't insert the event into the list, this is most
         * probably a non-recoverable error. Try to clean up best we
         * can.
         */
        if (!event_insert_success) {
          eventDestroy(current_event);
          eventListDestroy(list);
          file_error_result = FILE_INTERNAL_ERROR;
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

enum FileError save_calendar(const struct EventList *list,
                             const char *filename);

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
                                        struct CalendarFile *calendar_file) {
  enum FileError file_error_result;
  char date[EVENT_MAX_DATE_STR_LEN], time[EVENT_MAX_TIME_STR_LEN];
  int duration, read_result;

  file_error_result = FILE_NO_ERROR;

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
    size_t name_string_length, location_string_length;

    temp_name = NULL;
    temp_location = NULL;

    /*
     * Read in the event name string, variable length. The read buffer
     * will be expanded as necessary, up until it hits a newline.
     */
    file_error_result = readVariableLengthString(calendar_file);

    /*
     * If we don't get an error, then we continue processing. If we
     * do, we don't have to do anything else, just make sure the error
     * gets returned.
     */
    if (file_error_result == FILE_NO_ERROR) {
      name_string_length = strnlen(calendar_file->read_buffer,
                                   calendar_file->buffer_size);

      temp_name = (char *) malloc(name_string_length + 1);

      if (temp_name != NULL) {
        *temp_name = '\0';

        strncat(temp_name, calendar_file->read_buffer, name_string_length);
      }

      file_error_result = readVariableLengthString(calendar_file);

      location_string_length = strnlen(calendar_file->read_buffer,
                                       calendar_file->buffer_size);

      if (location_string_length > 0) {
        temp_location = (char *) malloc(location_string_length + 1);

        if (temp_location != NULL) {
          *temp_location = '\0';

          strncat(temp_location, calendar_file->read_buffer, location_string_length);
        }
      }

      /* Create the event */
      calendar_file->event_error = eventCreate(loaded_event, date, time, duration,
                                               temp_name, temp_location);

      if (temp_location != NULL) {
        free(temp_location);
      }

      if (temp_name != NULL) {
        free(temp_name);
      }
    } else {
      /*
       * We got a read error when we didn't expect it. If it was an
       * EOF, then the file format is wrong, otherwise it's an actual
       * read error.
       */
      if (file_error_result == FILE_EOF ) {
        file_error_result = FILE_INVALID_FORMAT;
      }
    }
  } else {
    /*
     * Didn't get the start of an event when we expected one.
     * If we get an EOF here, it probably means that we've got no more entries.
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
 * From the current position of the file, read in a variable length string.
 * up to a newline character, or end of file.
 *
 * An EOF here is not an error, but has to be returned as a separate
 * code so the caller knows to stop reading the file.
 */
static enum FileError readVariableLengthString(struct CalendarFile
                                               *calendar_file) {
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
      } else {
        error_result = FILE_ERROR;
      }
    }
  }

  return error_result;
}
