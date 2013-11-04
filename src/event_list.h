/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Linked list for handling the calendar events.
 */

#ifndef EVENT_LIST_H_
#define EVENT_LIST_H_

#include "bool.h"
#include "event.h"

/*
 * Just using a double ended linked list for events. Since we need to
 * add new events to the end of the list, and need to go through the
 * list in order.
 */
struct EventListNode {
  struct Event *event;
  struct EventListNode *next;
};

struct EventList {
  struct EventListNode *head;
  struct EventListNode *tail;
  struct EventListNode *current;
};

/*
 * Create a new event list and return a pointer to it.
 */
struct EventList *eventListCreate();

/*
 * Destroy the given list.
 *
 * Will clean up all the events, and free all the memory.
 */
void eventListDestroy(struct EventList *list);

/*
 * Insert the given event into the end of the list.
 */
Boolean eventListInsertLast(struct EventList *list,
                            struct Event *to_insert);

/*
 * Reset the internal iterator, so that eventListNext will return the
 * first event in the list.
 */
void eventListResetPosition(struct EventList *list);

/*
 * When called, returns the next event in the list. It is expected
 * that the list has been reset to the start, via
 * eventListResetPosition.
 */
struct Event *eventListNext(struct EventList *list);

/*
 * Returns the event formatted as specified in the assignment spec.
 */
char *eventListString(struct EventList *list);

/*
 * Search for event.
 *
 * Given a string, search for the given event name that matches that
 * string and return a pointer for that event.
 *
 * Returns NULL is the event wasn't found.
 *
 * Name must match exactly.
 */
struct Event *eventListFind(struct EventList *list, char *search_string);

#endif
