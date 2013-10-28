/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 */

#include <stdio.h>

#include "calendar_file.h"
#include "event_list.h"

/*
 * Load the given calendar file into the list.
 *
 * The list must be initalised.
 */
enum FileError load_calendar(struct EventList *list,
                             const char *filename) {
  FileError error_result;
  FILE *file_to_load;

  error_result = FILE_NO_ERROR;

  file_to_load = fopen(filename, "rb");

  if (file_to_load != NULL) {
  } else {
    error_result = FILE_ERROR;
  }
}

enum FileError save_calendar(const struct EventList *list,
                             const char *filename);
