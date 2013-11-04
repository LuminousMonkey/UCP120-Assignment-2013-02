/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#include <stdio.h>

#include "assignment_state.h"
#include "calendar_file.h"
#include "ui_assignment.h"

/*
 * If given a single command line argument, the program will try to
 * load that file as a calendar. If there is a problem, then it will
 * let the user know, and proceed with just an empty calendar.
 */
int main(int argc, char *argv[])
{
  /*
   * Our application state, this is passed around from function to
   * function, like a bad comedy act.
   */
  struct AssignmentState state;

  state.event_list = eventListCreate();
  state.error = NULL;
  state.error_code = 0;

  /*
   * If we get passed in a single argument, then try to load it in as
   * a calendar.
   */
  if (argc == 2) {
    enum FileError load_result;

    load_result = loadCalendar(state.event_list, argv[1]);

    /*
     * Check calendar_file.h for details on FILE_EOF.
     */
    if (load_result != FILE_NO_ERROR && load_result != FILE_EOF) {
      state.error =
        "There was an loading calendar file, starting with empty calendar.";
    }
  }

  uiSetup(&state);
  uiRun(&state);
  uiCleanup(&state);
  eventListDestroy(state.event_list);

  return state.error_code;
}
