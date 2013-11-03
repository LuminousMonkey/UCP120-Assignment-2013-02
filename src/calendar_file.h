/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Functions for reading from, and writing to the calendar files.
 *
 * These functions expect an initialised event list and will load the
 * given file into it.
 */

#ifndef CALENDAR_FILE_H_
#define CALENDAR_FILE_H_

#include "event_list.h"

/* Max filepath we support, anything longer will be truncated. */
#define MAX_FILENAME_LENGTH 256

/*
 * Used for reporting errors back to the caller.
 *
 * No tests for seeing if we can read or write to a file. Either we
 * are able to open the file, or we can't. If we can't, then it will
 * return a non-specific FILE_ERROR.
 *
 * FILE_NOTHING_READ is returned when an empty string has been read.
 *
 * FILE_INTERNAL_ERROR is a non-expected, probably non-recoverable error.
 * Try your best to clean up as much as possible.
 */
enum FileError {
  FILE_NO_ERROR,
  FILE_EOF,
  FILE_ERROR,
  FILE_INVALID_FORMAT,
  FILE_NO_FILENAME,
  FILE_EMPTY_LIST,
  FILE_INTERNAL_ERROR
};

/*
 * Load a calendar file into the given EventList.
 * Any existing data in this list will be destroyed.
 *
 * list - Pointer to a list already created with eventListCreate.
 * filename - A string of the calendar file to load.
 *
 * Returns and error status, FILE_NO_ERROR and FILE_EOF mean there
 * were no errors.
 *
 * If there was an error, then it's up to the caller of this function to
 * clean up and create a new empty list.
 */
enum FileError loadCalendar(struct EventList *list,
                            const char *filename);

/*
 * Save the given event list into a calendar file.
 *
 * list - Pointer to a list containing the events to save. Passing a
 *        null, or empty list will not save anything.
 *
 * filename - A string of the calendar file to save.
 */
enum FileError saveCalendar(struct EventList *list,
                            const char *filename);

/*
 * Given a file error, return a string that at least describes the
 * error a little bit.
 */
char *errorString(enum FileError file_error);

#endif
