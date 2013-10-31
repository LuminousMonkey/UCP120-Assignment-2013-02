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

enum FileError
{
  FILE_NO_ERROR,
  FILE_NOT_EXIST,
  FILE_NOT_READABLE,
  FILE_NOT_WRITABLE,
  FILE_EOF,
  FILE_ERROR,
  FILE_INVALID_FORMAT,
  FILE_INTERNAL_ERROR
};

enum FileError loadCalendar( struct EventList *list,
                             const char *filename );

enum FileError saveCalendar( const struct EventList *list,
                             const char *filename );

#endif
