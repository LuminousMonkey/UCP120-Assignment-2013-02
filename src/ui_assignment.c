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
#define QUIT_BUTTON_LABEL "Quit"

#define LOAD_CALENDAR_TITLE "Load Calendar"
#define SAVE_CALENDAR_TITLE "Save Calendar"

/*
 * Forward declarations
 */
static void uiLoadCalendar(void *in_data);
static void uiSaveCalendar(void *in_data);
static void uiShowError(struct AssignmentState *state);
static void uiSetCalendarText(struct AssignmentState *state);

/*
 * UI Setup.
 *
 * Takes a pointer to the AssignmentState struct. This struct is
 * passed around on all the button callbacks, it holds the main state
 * of the application, and is the main method of communication between
 * the different functions.
 */
void uiSetup(struct AssignmentState *state) {
  state->main_window = createWindow(MAIN_WINDOW_TITLE);

  uiSetCalendarText(state);

  addButton(state->main_window, LOAD_BUTTON_LABEL, &uiLoadCalendar, (void *)state);
  addButton(state->main_window, SAVE_BUTTON_LABEL, &uiSaveCalendar, (void *)state);
  /* addButton(state->main_window, ADD_BUTTON_LABEL, &uiAddEvent, (void *)state);
     addButton(state->main_window, EDIT_BUTTON_LABEL, &uiEditEvent, (void *)state);
     addButton(state->main_window, DELETE_BUTTON_LABEL, &uiDeleteEvent, (void *)state); */
  addButton(state->main_window, QUIT_BUTTON_LABEL, NULL, NULL);

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

static void uiClearCalendarText(struct AssignmentState *state) {
  setText(state->main_window, "");
}

static void uiLoadCalendar(void *in_data) {
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
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

  if (TRUE == dialogBox(state->main_window, LOAD_CALENDAR_TITLE, 1, &dialog_properties,
                        dialog_inputs)) {

    eventListDestroy(state->event_list);
    state->event_list = eventListCreate();

    file_error = loadCalendar(state->event_list, file_name);

    free(file_name);

    if (file_error == FILE_NO_ERROR) {
      uiSetCalendarText(state);
    } else {
      /*
       * There was an error with the file.
       * Clear out and create new event list.
       */
      eventListDestroy(state->event_list);
      state->event_list = eventListCreate();
      state->error = errorString(file_error);
      uiClearCalendarText(state);
      uiShowError(state);
    }
  }
}

static void uiSaveCalendar(void *in_data) {
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  InputProperties dialog_properties;
  char **dialog_inputs;
  char *file_name;
  enum FileError file_error;

  file_name = (char *)calloc(1, MAX_FILENAME_LENGTH + 1);
  assert(file_name);

  dialog_inputs = &file_name;

  dialog_properties.label = SAVE_CALENDAR_TITLE;
  dialog_properties.maxLength = MAX_FILENAME_LENGTH;
  dialog_properties.isMultiLine = FALSE;

  if (TRUE == dialogBox(state->main_window, SAVE_CALENDAR_TITLE, 1, &dialog_properties,
                        dialog_inputs)) {

    if (state->event_list->head != NULL) {
      file_error = saveCalendar(state->event_list, file_name);
    } else {
      file_error = FILE_EMPTY_LIST;
    }

    free(file_name);

    if (file_error != FILE_NO_ERROR) {
      state->error = errorString(file_error);
      uiShowError(state);
    }
  }
}

/*
 * Shows a message box to the user with the current error string in
 * the application state. Will then clear the error from the state.
 * Does no deallocation of memory, it just expects a pointer to an
 * already valid string.
 */
static void uiShowError(struct AssignmentState *state) {
  if (state->error != NULL) {
    messageBox(state->main_window, (char *)state->error);
    state->error = NULL;
  }
}
