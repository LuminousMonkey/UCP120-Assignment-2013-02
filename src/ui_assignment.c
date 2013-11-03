/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#include <assert.h>
#include <stdlib.h>

#include "assignment_state.h"
#include "calendar_file.h"
#include "event_list.h"
#include "ui_assignment.h"

#define MAIN_WINDOW_TITLE "UCP 120 Assignment"

#define LOAD_BUTTON_LABEL "Load"
#define SAVE_BUTTON_LABEL "Save"
#define ADD_BUTTON_LABEL "Add"
#define EDIT_BUTTON_LABEL "Edit"
#define DELETE_BUTTON_LABEL "Delete"

#define LOAD_CALENDAR_TITLE "Load Calendar"


/*
 * Forward declarations
 */
static void uiLoadCalendar(void *in_data);
static void uiShowError(struct AssignmentState *state);
static void uiSetCalendarText(struct AssignmentState *state);

void uiSetup(struct AssignmentState *state) {
  state->main_window = createWindow(MAIN_WINDOW_TITLE);

  uiSetCalendarText(state);

  addButton(state->main_window, LOAD_BUTTON_LABEL, &uiLoadCalendar,
            (void *)state);
  /*  addButton(state->main_window, SAVE_BUTTON_LABEL, &uiSaveCalendar, (void *)state);
  addButton(state->main_window, ADD_BUTTON_LABEL, &uiAddCalendar, (void *)state);
  addButton(state->main_window, EDIT_BUTTON_LABEL, &uiEditCalendar, (void *)state);
  addButton(state->main_window, DELETE_BUTTON_LABEL, &uiDeleteCalendar, (void *)state); */

  uiShowError(state);
}

void uiCleanup(struct AssignmentState *state) {
  freeWindow(state->main_window);
}

static void uiSetCalendarText(struct AssignmentState *state) {
  char *calendar_text;

  calendar_text = eventListString(state->event_list);

  if (calendar_text != NULL) {
    setText(state->main_window, calendar_text);
    free(calendar_text);
  }
}

static void uiLoadCalendar(void *inData) {
  struct AssignmentState *const state = (struct AssignmentState *)inData;
  InputProperties dialog_properties;
  char **dialog_inputs;
  char *file_name;
  enum FileError file_error;

  file_name = (char *)calloc(1, MAX_FILENAME_LENGTH + 1);
  assert(file_name);

  dialog_inputs = &file_name;

  dialog_properties.label = LOAD_CALENDAR_TITLE;
  dialog_properties.maxLength = MAX_FILENAME_LENGTH;
  dialog_properties.isMultiLine = FALSE;

  dialogBox(state->main_window, LOAD_CALENDAR_TITLE, 1, &dialog_properties,
            dialog_inputs);

  eventListDestroy(state->event_list);
  state->event_list = eventListCreate();

  file_error = loadCalendar(state->event_list, file_name);

  uiSetCalendarText(state);

  free(file_name);
}

/*
 * Shows a message box to the user with the current error string in
 * the application state. Will then clear the error from the state.
 * Does no deallocation of memory, it jsut expects a pointer to an
 * already valid string.
 */
static void uiShowError(struct AssignmentState *state) {
  if (state->error != NULL) {
    messageBox(state->main_window, (char *)state->error);
    state->error = NULL;
  }
}
