/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * This header contains the declaration for the assignment state. It's
 * just a struct where we keep the state of the application contained.
 * This is what gets passed around via the GUI functions, etc.
 */

#ifndef ASSIGNMENT_STATE_H_
#define ASSIGNMENT_STATE_H_

#include "event_list.h"
#include "gui.h"

struct AssignmentState {
  Window *main_window;
  struct EventList *event_list;
  const char *error;
};

#endif
