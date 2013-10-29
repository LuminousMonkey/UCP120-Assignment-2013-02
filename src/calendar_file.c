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

#define BUFFER_CHUNK 512

/*
 * Our struct for holding all the information about the current calendar file
 * we're loading.
 */
struct CalendarFile {
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
static enum FileError read_event_from_file(struct CalendarFile *calendar_file,
                                           struct Event **loaded_event);

/*
 * Load the given calendar file into the list.
 *
 * The list must be initalised.
 */
enum FileError load_calendar(struct EventList *list,
                             const char *filename) {
  enum FileError error_result;
  struct CalendarFile calendar_file;

  error_result = FILE_NO_ERROR;

  calendar_file.buffer_size = 0;
  calendar_file.current_file = fopen(filename, "rb");

  if (calendar_file.current_file != NULL) {
    struct Event *current_event;
    Boolean event_insert_success = FALSE;

    /* Allocate our starting buffer for reading variable length strings */
    calendar_file.read_buffer = (char *)malloc(BUFFER_CHUNK);

    if (calendar_file.read_buffer != NULL) {
      calendar_file.buffer_size = BUFFER_CHUNK;

      do {
        error_result = read_event_from_file(&calendar_file,
                                            &current_event);
        event_insert_success = event_list_insert_last(list, current_event);
      } while((calendar_file.event_error == EVENT_NO_ERROR) &&
              (error_result == FILE_NO_ERROR) &&
              event_insert_success);

      if (!event_insert_success) {
        event_destroy(current_event);
        event_list_destroy(list);
        error_result = FILE_INVALID_FORMAT;
      }

      free(calendar_file.read_buffer);
    } else {
      error_result = FILE_INTERNAL_ERROR;
    }

    fclose(calendar_file.current_file);
  } else {
    error_result = FILE_ERROR;
  }

  return error_result;
}

enum FileError save_calendar(const struct EventList *list,
                             const char *filename);

/*
 * From the current position of the file, tries to read in an
 * event.
 */
static enum FileError read_event_from_file(struct CalendarFile *calendar_file,
                                            struct Event **loaded_event) {
  enum FileError error_result;
  char date[EVENT_MAX_DATE_STR_LEN], time[EVENT_MAX_TIME_STR_LEN];
  int duration, read_result;

  /*
   * Scan in the first line of the event.
   *
   * Be sure to check event.h, EVENT_MAX_DATE_STR_LEN and
   * EVENT_MAX_TIME_STR_LEN determine the values to use in this format
   * string.
   */
  read_result = fscanf(calendar_file->current_file, EVENT_LEADING_FORMAT,
                       date, time, &duration);

  if (read_result == EVENT_LEADING_FORMAT_QTY) {
    int length;
    /* Read the rest of the line, it's the name */
    fgets(calendar_file->read_buffer, calendar_file->buffer_size,
          calendar_file->current_file);

    length = strnlen(calendar_file->read_buffer,calendar_file->buffer_size);
    if (calendar_file->read_buffer[length - 1] == '\n') {
      calendar_file->read_buffer[length - 1] = '\0';
    } else {
      /* Discard rest of line */
    }

    /* Create the event */
    calendar_file->event_error = event_create(loaded_event, date, time, duration,
                                              calendar_file->read_buffer, NULL);
  } else {
    error_result = FILE_ERROR;
  }

  return FILE_NO_ERROR;;
}

/*
 * From the current position of the file, read in a variable length string.
 * up to a newline character, or end of file.
 */
static enum FileError read_variable_string(struct CalendarFile *calendar_file) {
  return FILE_NO_ERROR;
}
