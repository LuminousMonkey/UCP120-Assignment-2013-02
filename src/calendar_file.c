/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "calendar_file.h"
#include "event_list.h"

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
static enum EventError read_event_from_file(struct CalendarFile *calendar_file,
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

  calendar_file.current_file = fopen(filename, "rb");

  if (calendar_file.current_file != NULL) {
    struct Event *current_event;
    Boolean event_insert_error = FALSE;

    do {
      calendar_file.event_error = read_event_from_file(&calendar_file,
                                                       &current_event);
      event_insert_error = event_list_insert_last(list, current_event);
    } while((calendar_file.event_error == FILE_NO_ERROR) &&
            !event_insert_error);

    if (event_insert_error) {
      event_destroy(current_event);
      event_list_destroy(list);
      error_result = FILE_INVALID_FORMAT;
    }

    fclose(calendar_file.current_file);

    if (calendar_file.read_buffer != NULL) {
      free(calendar_file.read_buffer);
    }
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
static enum EventError read_event_from_file(struct CalendarFile *calendar_file,
                                            struct Event **loaded_event) {

}

/*
 * From the current position of the file, read in a variable length string.
 * up to a newline character, or end of file.
 */
static enum FileError read_variable_string(struct CalendarFile *calendar_file)
