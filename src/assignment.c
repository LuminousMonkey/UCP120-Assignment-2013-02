/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Main file for the UCP assignment, just does simple command line
 * parsing before setting up and running the GUI.
 */

#include <stdio.h>

#include "assignment_state.h"
#include "calendar_file.h"
#include "date_time.h"
#include "event.h"
#include "event_list.h"
#include "ui_assignment.h"

#include "gui.h"

int main(int argc, char *argv[])
{
  struct AssignmentState state;

  state.event_list = eventListCreate();
  state.error = NULL;
  state.error_code = 0;

  if (argc == 2) {
    /* Try loading the calendar file. */
    enum FileError load_result;

    load_result = loadCalendar(state.event_list, argv[1]);

    if (load_result != FILE_NO_ERROR && load_result != FILE_EOF) {
      state.error =
        "There was an loading calendar file, starting with empty calendar.";
    }
  }

  uiSetup(&state);
  runGUI(state.main_window);
  uiCleanup(&state);
  eventListDestroy(state.event_list);

  return state.error_code;
}
