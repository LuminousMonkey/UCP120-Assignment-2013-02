/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#ifndef UI_ASSIGNMENT_H_
#define UI_ASSIGNMENT_H_

#include "assignment_state.h"
#include "event_list.h"
#include "gui.h"

/*
 * UI Setup.
 *
 * Takes a pointer to the AssignmentState struct. This struct is
 * passed around on all the button callbacks, it holds the main state
 * of the application, and is the main method of communication between
 * the different functions.
 */
void uiSetup(struct AssignmentState *state);
void uiRun(const struct AssignmentState *const state);
void uiCleanup(struct AssignmentState *state);

#endif
