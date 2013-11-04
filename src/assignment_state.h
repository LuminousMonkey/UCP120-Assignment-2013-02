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

/*
 * The assignment state.
 *
 * This is a struct that is passed around between all the GUI
 * functions, it represents the core state of the assignment
 * application. It just involves the Window type for the GUI, the
 * calendar events we have loaded, and an error string for informing
 * the user via the GUI. Finally we have the error_code that gets
 * returned if the application quits.
 *
 * The error string gets checked, and it just pops up that string in a
 * message box. The pointer will then get cleared, so this means we
 * cannot use dynamically allocated strings.
 */
struct AssignmentState {
  Window *main_window;
  struct EventList *event_list;
  const char *error;
  int error_code;
};

#endif
