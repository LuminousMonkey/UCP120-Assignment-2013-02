/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 */

#include <stdlib.h>
#include <string.h>

#include "event_list.h"

/*
 * String that is put at the end of the events in the calendar text.
 */
#define EVENT_END_TERMINATOR "\n---"

/*
 * Creates an empty list, returning a pointer to the list.
 */
struct EventList *eventListCreate()
{
  struct EventList *new_list;

  new_list = (struct EventList *) malloc(sizeof(struct EventList));

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
 *
 * It's up the the caller to set the list pointer to NULL (or to
 * create a new empty list).
 */
void eventListDestroy(struct EventList *list)
{
  struct EventListNode *current_node;
  struct EventListNode *next_node;

  current_node = list->head;

  while (current_node != NULL) {
    next_node = current_node->next;
    eventDestroy(current_node->event);
    current_node->next = NULL;
    free(current_node);
    current_node = next_node;
  }

  list->head = NULL;
  list->tail = NULL;

  free(list);
}

/*
 * Resets the list so the next call with "next" will return the first
 * event in the list.
 */
void eventListResetPosition(struct EventList *list)
{
  list->current = list->head;
}

/*
 * Returns the next event in the list.
 *
 * Returns NULL if no more events.
 * Be sure to use event_list_reset_position first.
 */
struct Event *eventListNext(struct EventList *list)
{
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
Boolean eventListInsertLast(struct EventList *list,
                            struct Event *to_insert)
{
  struct EventListNode *new_node;
  Boolean result;

  result = FALSE;

  if (to_insert != NULL) {
    new_node = (struct EventListNode *) malloc(sizeof(
                 struct EventListNode));

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

/*
 * Returns a pointer to the string of the entire calendar list,
 * formatted as specified in the assignment spec.
 *
 * It will allocate the memory, but it is expected that the caller
 * frees it using free().
 */
char *eventListString(struct EventList *list)
{
  char *result;
  result = NULL;

  if (list->head != NULL) {
    int current_size, num_of_events;
    struct Event *current_event;

    current_size = 0;
    num_of_events = 0;

    eventListResetPosition(list);
    current_event = eventListNext(list);

    while (current_event != NULL) {
      current_size += current_event->formatted_string_length;
      num_of_events++;
      current_event = eventListNext(list);
    }

    /*
     * Allocate memory for the whole string of formatted events.
     *
     * This includes 6 extra characters per entry for the formatting.
     * The last entry only has 5 (no line space. But has terminator.)
     * TODO: No Magic numbers.
     */
    current_size += (6 * num_of_events) - 1;

    result = (char *)malloc(current_size);

    if (result != NULL) {
      eventListResetPosition(list);
      current_event = eventListNext(list);
      *result = '\0';

      while (current_event != NULL) {
        strncat(result, current_event->formatted_string,
                current_event->formatted_string_length);

        if (list->current != NULL) {
          strcat(result, EVENT_END_TERMINATOR "\n\n");
        } else {
          strcat(result, EVENT_END_TERMINATOR);
        }

        current_event = eventListNext(list);
      }
    }
  }

  return result;
}

/*
 * Search for event.
 */
struct Event *eventListFind(struct EventList *list, char *search_string)
{
  struct Event *result;
  Boolean not_found;

  eventListResetPosition(list);
  result = eventListNext(list);

  not_found = TRUE;

  /*
   * Loop through the list, looking at each event for a match.
   */
  while (result != NULL && not_found) {
    if (strcmp(search_string, result->name) == 0) {
      not_found = FALSE;
    } else {
      result = eventListNext(list);
    }
  }

  return result;
}

Boolean eventListDelete(struct EventList *list, struct Event *to_delete) {
  Boolean node_found;

  struct EventListNode *previous_node;
  struct EventListNode *next_node;
  struct EventListNode *node_to_delete;

  node_found = FALSE;

  previous_node = NULL;

  /* Start with the first node */
  node_to_delete = list->head;

  /* Step through the nodes of the list */
  while (node_to_delete != NULL && !node_found) {
    /*
     * If this is the node we're going to remove, we need the pointer
     * to the next node.
     */
    next_node = node_to_delete->next;

    /* We have a match! */
    if (node_to_delete->event == to_delete) {
      /* If we had a previous node, link to the next node. */
      if (previous_node != NULL) {
        previous_node->next = next_node;
      }

      /* Are we deleting the head node? */
      if (list->head == node_to_delete) {
        /* Yep, so next node is the new head. */
        list->head = next_node;
      }

      /* Deleting the tail node? */
      if (list->tail == node_to_delete) {
        /* Yep, make tail the previous node. */
        list->tail = previous_node;
      }

      eventDestroy(node_to_delete->event);
      free(node_to_delete);

      node_found = TRUE;
    } else {
      /* Not the right one, keep looking. */
      previous_node = node_to_delete;
      node_to_delete = node_to_delete->next;
    }
  }

  return node_found;
}
