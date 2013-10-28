/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Functions for reading from, and writing to the calendar files.
 */

#ifndef CALENDAR_FILE_H_
#define CALENDAR_FILE_H_

#include "event_list.h"

enum FileError {
  FILE_NO_ERROR,
  FILE_NOT_EXIST,
  FILE_NOT_READABLE,
  FILE_NOT_WRITABLE,
  FILE_ERROR,
  FILE_INTERNAL_ERROR
};

enum FileError load_calendar(struct EventList *list,
                             const char *filename);

enum FileError save_calendar(const struct EventList *list,
                             const char *filename);

#endif
