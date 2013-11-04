/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * This is the user interface related code.
 */

#include <assert.h>
#include <stdlib.h>

#include "assignment_state.h"
#include "calendar_file.h"
#include "date_time.h"
#include "event.h"
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
#define ADD_EVENT_TITLE "Add Event"
#define EDIT_EVENT_TITLE "Edit Event"
#define DELETE_EVENT_TITLE "Delete Event"

#define EDIT_PROPERTIES(var_name) InputProperties var_name[] = {  \
                                                                  {"Event Name", MAX_LENGTH_OF_NAME, FALSE},                    \
                                                                  {"Location", MAX_LENGTH_OF_LOCATION, FALSE},                  \
                                                                  {"Date", MAX_DATE_STRING, FALSE},                             \
                                                                  {"Time", MAX_TIME_STRING, FALSE},                             \
                                                                  {"Duration", MAX_DURATION_STRING, FALSE}}

#define EDIT_PROPERTIES_SIZE 5

/*
 * Forward declarations
 */
static void uiLoadCalendar(void *in_data);
static void uiSaveCalendar(void *in_data);
static void uiAddEvent(void *in_data);
static void uiShowError(struct AssignmentState *state);
static void uiSetCalendarText(struct AssignmentState *state);
static void uiClearCalendarText(struct AssignmentState *state);

/*
 * UI Setup.
 *
 * Takes a pointer to the AssignmentState struct. This struct is
 * passed around on all the button callbacks, it holds the main state
 * of the application, and is the main method of communication between
 * the different functions.
 */
void uiSetup(struct AssignmentState *state)
{
  state->main_window = createWindow(MAIN_WINDOW_TITLE);

  uiSetCalendarText(state);

  addButton(state->main_window, LOAD_BUTTON_LABEL, &uiLoadCalendar,
            (void *)state);
  addButton(state->main_window, SAVE_BUTTON_LABEL, &uiSaveCalendar,
            (void *)state);
  addButton(state->main_window, ADD_BUTTON_LABEL, &uiAddEvent, (void *)state);
  /*   addButton(state->main_window, EDIT_BUTTON_LABEL, &uiEditEvent, (void *)state);
     addButton(state->main_window, DELETE_BUTTON_LABEL, &uiDeleteEvent, (void *)state); */
  addButton(state->main_window, QUIT_BUTTON_LABEL, NULL, NULL);

  uiShowError(state);
}

void uiCleanup(struct AssignmentState *state)
{
  freeWindow(state->main_window);
}

static void uiLoadCalendar(void *in_data)
{
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  InputProperties dialog_properties;
  char **dialog_inputs;
  char *file_name;

  file_name = (char *)calloc(1, MAX_FILENAME_LENGTH + 1);
  assert(file_name != NULL);

  dialog_inputs = &file_name;

  dialog_properties.label = LOAD_CALENDAR_TITLE;
  dialog_properties.maxLength = MAX_FILENAME_LENGTH;
  dialog_properties.isMultiLine = FALSE;

  if (TRUE == dialogBox(state->main_window, LOAD_CALENDAR_TITLE, 1,
                        &dialog_properties,
                        dialog_inputs)) {
    enum FileError file_error;

    eventListDestroy(state->event_list);
    state->event_list = eventListCreate();

    file_error = loadCalendar(state->event_list, file_name);

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

  free(file_name);
}

static void uiSaveCalendar(void *in_data)
{
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  InputProperties dialog_properties;
  char **dialog_inputs;
  char *file_name;

  file_name = (char *)calloc(1, MAX_FILENAME_LENGTH + 1);
  assert(file_name != NULL);
  dialog_inputs = &file_name;

  dialog_properties.label = SAVE_CALENDAR_TITLE;
  dialog_properties.maxLength = MAX_FILENAME_LENGTH;
  dialog_properties.isMultiLine = FALSE;

  if (TRUE == dialogBox(state->main_window, SAVE_CALENDAR_TITLE, 1,
                        &dialog_properties,
                        dialog_inputs)) {
    enum FileError file_error;

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

static void uiAddEvent(void *in_data)
{
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  EDIT_PROPERTIES(dialog_properties);
  char **dialog_inputs;
  char *name, *location, *date, *time, *duration;

  name = (char *)calloc(1, MAX_LENGTH_OF_NAME + 1);
  location = (char *)calloc(1, MAX_LENGTH_OF_LOCATION + 1);
  date = (char *)calloc(1, MAX_DATE_STRING + 1);
  time = (char *)calloc(1, MAX_TIME_STRING + 1);
  duration = (char *)calloc(1, MAX_DURATION_STRING + 1);

  if (name != NULL &&
      location != NULL &&
      date != NULL &&
      time != NULL &&
      duration != NULL) {
    /* Order is important, please look at EDIT_PROPERTIES #define */
    char *dialog_inputs[] = {name, location, date, time, duration};

    if (TRUE == dialogBox(state->main_window, ADD_EVENT_TITLE, EDIT_PROPERTIES_SIZE,
                          &dialog_properties, dialog_inputs)) {
      /* Try creating the event, if we fail, let the user know. */
      enum EventError error_result;
      struct Event *new_event;

      error_result = eventCreate(&new_event, date, time, atoi(duration),
                                 name, location);

      if (error_result == EVENT_NO_ERROR) {
        if (eventListInsertLast(state->event_list, new_event)) {
          /* No error, update calendar display. */
          uiSetCalendarText(state);
        } else {
          /* Error inserting into list */
          /* Free up the event. */
          state->error = "Error trying to add event to list.";
          free(new_event);
        }
      } else {
        /* Error creating the event. */
        state->error = "Error creating event, invalid fields?";
      }
    }
  }

  /* Clean up anything we might have allocated. */

  if (name != NULL) {
    free(name);
  }
  if (location != NULL) {
    free(location);
  }
  if (date != NULL) {
    free(date);
  }
  if (time != NULL) {
    free(time);
  }
  if (duration != NULL) {
    free(duration);
  }

  /* Show any errors */
  uiShowError(state);
}

/*
 * Shows a message box to the user with the current error string in
 * the application state. Will then clear the error from the state.
 * Does no deallocation of memory, it just expects a pointer to an
 * already valid string.
 */
static void uiShowError(struct AssignmentState *state)
{
  if (state->error != NULL) {
    messageBox(state->main_window, (char *)state->error);
    state->error = NULL;
  }
}

static void uiSetCalendarText(struct AssignmentState *state)
{
  char *calendar_text;

  calendar_text = eventListString(state->event_list);

  if (calendar_text != NULL) {
    setText(state->main_window, calendar_text);
    free(calendar_text);
  }
}

static void uiClearCalendarText(struct AssignmentState *state)
{
  setText(state->main_window, "");
}
