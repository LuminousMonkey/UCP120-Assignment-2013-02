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
struct EventListNode
  {
  struct Event *event;
  struct EventListNode *next;
  };

struct EventList
  {
  struct EventListNode *head;
  struct EventListNode *tail;
  struct EventListNode *current;
  };

struct EventList *eventListCreate();
void eventListDestroy( struct EventList *list );

Boolean eventListInsertLast( struct EventList *list,
                             struct Event *to_insert );

void eventListResetPosition( struct EventList *list );
struct Event *eventListNext( struct EventList *list );

#endif
