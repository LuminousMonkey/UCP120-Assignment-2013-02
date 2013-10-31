/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 */

#include <stdlib.h>

#include "event_list.h"

/*
 * Creates an empty list, returning a pointer to the list.
 */
struct EventList *eventListCreate() {
  struct EventList *new_list;

  new_list = (struct EventList *)malloc(sizeof(struct EventList));

  if (new_list != NULL) {
    new_list->head = NULL;
    new_list->tail = NULL;
    new_list->current = NULL;
  }

  return new_list;
}

/*
 * Destroys a list.
 *
 * This will also destroy any Events stored on that list, since we
 * have no way of knowing if the pointers for the Events are stored
 * anywhere else in the program.
 */
void eventListDestroy(struct EventList *list) {
  struct EventListNode *current_node;
  struct EventListNode *next_node;

  current_node = list->head;

  while (current_node != NULL) {
    next_node = current_node->next;
    event_destroy(current_node->event);
    free(current_node);
    current_node = next_node;
  }

  free(list);
}

/*
 * Resets the list so the next call with "next" will return the first
 * event in the list.
 */
void eventListResetPosition(struct EventList *list) {
  list->current = list->head;
}

/*
 * Returns the next event in the list.
 *
 * Returns NULL if no more events.
 * Be sure to use event_list_reset_position first.
 */
struct Event *eventListNext(struct EventList *list) {
  struct Event *result;

  result = NULL;

  if (list->current != NULL) {
    result = list->current->event;
    list->current = list->current->next;
  }

  return result;
}

/*
 * Given a list, add the given event to that list.
 */
Boolean eventListInsertLast(struct EventList *list, struct Event *to_insert) {
  struct EventListNode *new_node;
  Boolean result;

  result = FALSE;
  if (to_insert != NULL) {
    new_node = (struct EventListNode *)malloc(sizeof(struct EventListNode));

    if (new_node != NULL) {
      new_node->event = to_insert;
      new_node->next = NULL;

      /*
       * First item inserted into list.
       */
      if (list->head == NULL) {
        list->head = new_node;
      }

      if (list->tail != NULL) {
        list->tail->next = new_node;
      }

      list->tail = new_node;
      result = TRUE;
    }
  }

  return result;
}
