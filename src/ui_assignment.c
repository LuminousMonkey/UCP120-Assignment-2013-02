/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * This is the user interface related code.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assignment_state.h"
#include "calendar_file.h"
#include "date_time.h"
#include "event.h"
#include "event_list.h"
#include "ui_assignment.h"

#define MAIN_WINDOW_TITLE "UCP 120 Assignment"

/*
 * Our labels for the buttons.
 */
#define LOAD_BUTTON_LABEL "Load"
#define SAVE_BUTTON_LABEL "Save"
#define ADD_BUTTON_LABEL "Add"
#define EDIT_BUTTON_LABEL "Edit"
#define DELETE_BUTTON_LABEL "Delete"
#define QUIT_BUTTON_LABEL "Quit"

/*
 * Titles for the different dialog boxes a user might get.
 */
#define LOAD_CALENDAR_TITLE "Load Calendar"
#define SAVE_CALENDAR_TITLE "Save Calendar"
#define ADD_EVENT_TITLE "Add Event"
#define EDIT_EVENT_TITLE "Edit Event"
#define DELETE_EVENT_TITLE "Delete Event"
#define FIND_EVENT_TITLE "Find Event"

/*
 * Label for field that prompts for the event name.
 * Shown whenever user it prompted to find an event.
 */
#define EVENT_NAME_PROMPT "Event name (exact, case-sensitive)"

/*
 * Both the addEvent, and editEvent functions share the layout for the fields of an event.
 * This macro is used to create the variable that holds those properties.
 */
#define EDIT_PROPERTIES(var_name) InputProperties var_name[] = {  \
    {"Event Name", MAX_LENGTH_OF_NAME, FALSE},                          \
    {"Location", MAX_LENGTH_OF_LOCATION, FALSE},                        \
    {"Date (YYYY-MM-DD)", MAX_DATE_STRING, FALSE},                      \
    {"Time", MAX_TIME_STRING, FALSE},                                   \
    {"Duration", MAX_DURATION_STRING, FALSE}}

/*
 * The index of the strings to be edited passed in via
 * InputProperties.
 *
 * These need to be updated if the ordering of the fields in the
 * EDIT_PROPERTIES macro changes.
 */
#define NAME_INDEX 0
#define LOCATION_INDEX 1
#define DATE_INDEX 2
#define TIME_INDEX 3
#define DURATION_INDEX 4

/* Number of fields we have for the edit/add event fields */
#define EDIT_PROPERTIES_SIZE 5

/*
 * This struct is used to hold the event details for the dialog box
 * editing. Because both Add, and Edit events have a few bits of
 * common code, this struct will hold the variables we need so we can
 * generalise the functions a bit more.
 *
 * name - Pointer to string for the name in the dialog.
 * location - Pointer to string for the location.
 * date - Pointer to string for the date.
 * time - Pointer to string for the time.
 * duration - Pointer to string for the duration.
 */
struct DialogEventFields {
  char *name;
  char *location;
  char *date;
  char *time;
  char *duration;
};

/* Forward declarations. */
/* The UI button callbacks. */
static void uiLoadCalendar(void *in_data);
static void uiSaveCalendar(void *in_data);
static void uiAddEvent(void *in_data);
static void uiEditEvent(void *in_data);
static void uiDeleteEvent(void *in_data);

/* Utility functions. */
static struct Event *uiFindEvent(struct AssignmentState *const state);
static void uiShowError(struct AssignmentState *const state);
static void uiSetCalendarText(struct AssignmentState *state);
static void uiClearCalendarText(struct AssignmentState *state);

/* Functions for the add/edit dialogs */
static void createEventDialogFieldStrings(struct DialogEventFields *fields);
static void destroyEventDialogFieldStrings(struct DialogEventFields *fields);
static void mapEventDialogFieldStringsToInputs(char *inputs[],
                                               struct DialogEventFields *fields);


/*
 * UI Setup.
 *
 * Takes a pointer to the AssignmentState struct. This struct is
 * passed around on all the button callbacks, it holds the main state
 * of the application, and is the main method of communication between
 * the different functions.
 */
void uiSetup(struct AssignmentState *const state)
{
  state->main_window = createWindow(MAIN_WINDOW_TITLE);

  uiSetCalendarText(state);

  addButton(state->main_window, LOAD_BUTTON_LABEL, &uiLoadCalendar,
            (void *)state);
  addButton(state->main_window, SAVE_BUTTON_LABEL, &uiSaveCalendar,
            (void *)state);
  addButton(state->main_window, ADD_BUTTON_LABEL, &uiAddEvent, (void *)state);
  addButton(state->main_window, EDIT_BUTTON_LABEL, &uiEditEvent, (void *)state);
  addButton(state->main_window, DELETE_BUTTON_LABEL, &uiDeleteEvent, (void *)state);
  addButton(state->main_window, QUIT_BUTTON_LABEL, NULL, NULL);

  uiShowError(state);
}

/*
 * UI Run
 *
 * This is just a simple wrapper, just gets rid of a tiny bit of
 * coupling between the main assignment function and the GUI lib
 * provided by Dave.
 */
void uiRun(const struct AssignmentState *const state)
{
  runGUI(state->main_window);
}

/*
 * UI Cleanup.
 *
 * Like UI Run, it's just a simple wrapper.
 */
void uiCleanup(const struct AssignmentState *const state)
{
  freeWindow(state->main_window);
}

/*
 * UI Load Calendar
 *
 * When the user presses the Load Calendar button, this is called,
 * getting passed the application state.
 *
 * This will then prompt the user for the calendar file, and try to
 * load the calendar into memory.
 *
 * If there is an error any loaded calendars will be destroyed.
 * This is not a very user friendly thing to do.
 *
 * If there is an error, then it will be displayed to the user.
 *
 * in_data - Pointer to the application state.
 */
static void uiLoadCalendar(void *in_data)
{
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  InputProperties dialog_properties;
  char **dialog_inputs;
  char *file_name;

  /* Allocate the space we need for the filename prompt. */
  file_name = (char *)malloc(MAX_FILENAME_LENGTH + 1);
  assert(file_name != NULL);

  /* Make sure the GUI considers the string "empty" */
  *file_name = '\0';

  /* We just have the one input. */
  dialog_inputs = &file_name;

  dialog_properties.label = LOAD_CALENDAR_TITLE;
  dialog_properties.maxLength = MAX_FILENAME_LENGTH;
  dialog_properties.isMultiLine = FALSE;

  /*
   * Try loading the file, if the user clicks "OK", otherwise we do
   * nothing else but return to the main window.
   */
  if (TRUE == dialogBox(state->main_window, LOAD_CALENDAR_TITLE, 1,
                        &dialog_properties,
                        dialog_inputs)) {
    enum FileError file_error;

    /* Clear off the current calendar. */
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
      state->error = calendarErrorString(file_error);
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
      state->error = calendarErrorString(file_error);
      uiShowError(state);
    }
  }
}

static void uiAddEvent(void *in_data)
{
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  EDIT_PROPERTIES(dialog_properties);
  struct DialogEventFields dialog_fields;
  createEventDialogFieldStrings(&dialog_fields);

  if (dialog_fields.name != NULL &&
      dialog_fields.location != NULL &&
      dialog_fields.date != NULL &&
      dialog_fields.time != NULL &&
      dialog_fields.duration != NULL) {

    char *dialog_inputs[EDIT_PROPERTIES_SIZE];
    mapEventDialogFieldStringsToInputs(dialog_inputs, &dialog_fields);

    if (TRUE == dialogBox(state->main_window, ADD_EVENT_TITLE, EDIT_PROPERTIES_SIZE,
                          (InputProperties *)&dialog_properties, dialog_inputs)) {
      /* Try creating the event, if we fail, let the user know. */
      enum EventError error_result;
      struct Event *new_event;

      error_result = eventCreate(&new_event, dialog_fields.date,
                                 dialog_fields.time,
                                 atoi(dialog_fields.duration),
                                 dialog_fields.name,
                                 dialog_fields.location);

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

  destroyEventDialogFieldStrings(&dialog_fields);

  /* Show any errors */
  uiShowError(state);
}

/*
 * Prompts the user for an event to edit.
 *
 * Searches for that event, if found, pops up an edit window.
 */
static void uiEditEvent(void *in_data)
{
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  struct Event *event_to_edit;
  EDIT_PROPERTIES(dialog_properties);
  struct DialogEventFields dialog_fields;

  event_to_edit = uiFindEvent(state);

  if (event_to_edit != NULL) {
    createEventDialogFieldStrings(&dialog_fields);

    if (dialog_fields.name != NULL &&
        dialog_fields.location != NULL &&
        dialog_fields.date != NULL &&
        dialog_fields.time != NULL &&
        dialog_fields.duration != NULL) {

      char *dialog_inputs[EDIT_PROPERTIES_SIZE];

      /* Copy the found event strings over to the edit box */
      strncat(dialog_fields.name, event_to_edit->name, MAX_LENGTH_OF_NAME);
      strncat(dialog_fields.location, event_to_edit->location,
              MAX_LENGTH_OF_LOCATION);

      sprintf(dialog_fields.date, "%04d-%02d-%02d",
              event_to_edit->date.year,
              event_to_edit->date.month,
              event_to_edit->date.day);

      sprintf(dialog_fields.time, "%02d:%02d",
              event_to_edit->time.hour,
              event_to_edit->time.minutes);

      sprintf(dialog_fields.duration, "%d", event_to_edit->duration);

      mapEventDialogFieldStringsToInputs(dialog_inputs, &dialog_fields);

      if (TRUE == dialogBox(state->main_window, ADD_EVENT_TITLE,
                            EDIT_PROPERTIES_SIZE,
                            (InputProperties *)&dialog_properties,
                            dialog_inputs)) {
        enum EventError error_result;

        error_result = eventEdit(event_to_edit, dialog_fields.date,
                                 dialog_fields.time,
                                 atoi(dialog_fields.duration),
                                 dialog_fields.name,
                                 dialog_fields.location);

        if (error_result == EVENT_NO_ERROR) {
          uiSetCalendarText(state);
        } else {
          /* Error creating the event. */
          state->error = "Error editing event, invalid fields?";
        }
      }
    }

    destroyEventDialogFieldStrings(&dialog_fields);
  }

  /* Show any errors */
  uiShowError(state);
}

/*
 * Prompt the user for an event name, and delete the event.
 */
static void uiDeleteEvent(void *in_data) {
  struct AssignmentState *const state = (struct AssignmentState *)in_data;
  struct Event *event_to_delete;

  event_to_delete = uiFindEvent(state);

  if (event_to_delete != NULL) {
    if (!eventListDelete(state->event_list, event_to_delete)) {
      state->error = "Found event, but couldn't delete it.";
    } else {
      /* Delete worked, update the calendar display. */
      uiSetCalendarText(state);
    }
  } else {
    state->error = "Could not find event to delete";
  }

  /* Show any errors */
  uiShowError(state);
}

/*
 * Prompts the user for an event name.
 *
 * Returns a pointer to the event, NULL if no event was found.
 */
static struct Event *uiFindEvent(struct AssignmentState *const state)
{
  InputProperties dialog_properties;
  struct Event *result;
  char **dialog_inputs;
  char *event_name_to_find;

  event_name_to_find = (char *)calloc(1, MAX_LENGTH_OF_NAME + 1);
  assert(event_name_to_find != NULL);
  dialog_inputs = &event_name_to_find;

  dialog_properties.label = EVENT_NAME_PROMPT;
  dialog_properties.maxLength = MAX_LENGTH_OF_NAME;
  dialog_properties.isMultiLine = FALSE;

  if (TRUE == dialogBox(state->main_window, FIND_EVENT_TITLE, 1,
                        &dialog_properties,
                        dialog_inputs)) {
    result = eventListFind(state->event_list, event_name_to_find);
    if (result == NULL) {
      state->error = "Could not find event.";
    }
  } else {
    result = NULL;
  }

  free(event_name_to_find);

  return result;
}

/*
 * Shows a message box to the user with the current error string in
 * the application state. Will then clear the error from the state.
 * Does no deallocation of memory, it just expects a pointer to an
 * already valid string.
 */
static void uiShowError(struct AssignmentState *const state)
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
  } else {
    setText(state->main_window, "");
  }
}

static void uiClearCalendarText(struct AssignmentState *state)
{
  setText(state->main_window, "");
}

/*
 * Allocate the memory for the strings for the add/edit event dialog
 * box.
 */
static void createEventDialogFieldStrings(struct DialogEventFields *fields) {
  assert(fields != NULL);
  fields->name = (char *)calloc(1, MAX_LENGTH_OF_NAME + 1);
  fields->location = (char *)calloc(1, MAX_LENGTH_OF_LOCATION + 1);
  fields->date = (char *)calloc(1, MAX_DATE_STRING + 1);
  fields->time = (char *)calloc(1, MAX_TIME_STRING + 1);
  fields->duration = (char *)calloc(1, MAX_DURATION_STRING + 1);
}

/*
 * Map the dialog field strings to the dialog_inputs array that we
 * need to pass into the dialogBox function.
 *
 * inputs - Pointer to the array that has to be populated with the
 *          string pointers.
 * fields - The DialogEventFields struct that holds the strings for
 *           the event.
 */
static void mapEventDialogFieldStringsToInputs(char *inputs[],
                                               struct DialogEventFields *fields) {
  inputs[NAME_INDEX] = fields->name;
  inputs[LOCATION_INDEX] = fields->location;
  inputs[DATE_INDEX] = fields->date;
  inputs[TIME_INDEX] = fields->time;
  inputs[DURATION_INDEX] = fields->duration;

}

/*
 * Free the memory for the strings for the add/edit event dialog box.
 */
static void destroyEventDialogFieldStrings(struct DialogEventFields *fields) {
  assert(fields != NULL);
  free(fields->name);
  free(fields->location);
  free(fields->date);
  free(fields->time);
  free(fields->duration);
}
