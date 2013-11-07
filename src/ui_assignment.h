/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * This is our GUI code, this code sets the main window and buttons
 * up. When the buttons are pressed it will do the necessary callbacks
 * to do what the user wants.
 */

#ifndef UI_ASSIGNMENT_H_
#define UI_ASSIGNMENT_H_

#include "assignment_state.h"

/*
 * UI Setup.
 *
 * Takes a pointer to the state struct, this will initialise the state.
 * This includes creating the GUI window and showing any errors.
 */
void uiSetup(struct AssignmentState *const state);

/*
 * UI Run
 *
 * This starts the application running.
 *
 * state - Application state.
 */
void uiRun(const struct AssignmentState *const state);

/*
 * UI Cleanup
 *
 * Frees up the GUI window.
 */
void uiCleanup(const struct AssignmentState *const state);

#endif
