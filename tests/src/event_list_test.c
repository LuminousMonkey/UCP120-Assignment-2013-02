/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#include <CUnit/CUnit.h>

#include "event_list_test.h"

void testEventListCreateList() {
  struct EventList *test_list;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL(test_list);

  CU_ASSERT_PTR_NULL(test_list->head);
  CU_ASSERT_PTR_NULL(test_list->tail);
  CU_ASSERT_PTR_NULL(test_list->current);

  eventListDestroy(test_list);
}

void testEventListInsertSingle() {
  struct EventList *test_list;
  struct Event *test_event_one;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL(test_list);

  eventCreate(&test_event_one, "2010-05-24", "07:15", 10, "Event 1", NULL);

  eventListInsertLast(test_list, test_event_one);

  CU_ASSERT_PTR_NOT_NULL(test_list->head);
  CU_ASSERT_PTR_NOT_NULL(test_list->tail);
  CU_ASSERT_PTR_NULL(test_list->current);

  CU_ASSERT_PTR_EQUAL(test_list->head, test_list->tail);

  eventListDestroy(test_list);
}

void testEventListInsertMultiple() {
  struct EventList *test_list;
  struct Event *test_event_one, *test_event_two,
      *test_event_three, *compare_event;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL(test_list);

  eventCreate(&test_event_one, "2010-05-24", "06:15", 10, "Event 1", NULL);
  eventCreate(&test_event_two, "2011-05-24", "06:15", 10, "Event 2", NULL);
  eventCreate(&test_event_three, "2012-05-24", "06:15", 10, "Event 3", NULL);

  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_one));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_two));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_three));

  eventListResetPosition(test_list);
  compare_event = eventListNext(test_list);

  CU_ASSERT_PTR_EQUAL(test_event_one, compare_event);

  compare_event = eventListNext(test_list);
  CU_ASSERT_PTR_EQUAL(test_event_two, compare_event);

  compare_event = eventListNext(test_list);
  CU_ASSERT_PTR_EQUAL(test_event_three, compare_event);

  eventListDestroy(test_list);
}

/* Inserting a null event should do nothing to the list. */
void testEventListInsertNull() {
  struct EventList *test_list;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL(test_list);

  CU_ASSERT_EQUAL(eventListInsertLast(test_list, NULL), FALSE);

  /* Just check that the head and tail are still NULL */
  CU_ASSERT_PTR_NULL(test_list->head);
  CU_ASSERT_PTR_NULL(test_list->tail);
}

void testEventListFind() {
  struct EventList *test_list;
  struct Event *test_event_one, *test_event_two, *test_event_three,
      *found_event;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  eventCreate(&test_event_one, "2010-05-24", "06:15", 10, "Event 1", NULL);
  eventCreate(&test_event_two, "2011-05-24", "06:15", 10, "Event 2", NULL);
  eventCreate(&test_event_three, "2012-05-24", "06:15", 10, "Event 3", NULL);

  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_one));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_two));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_three));

  found_event = eventListFind(test_list, "Event 1");

  CU_ASSERT_PTR_NOT_NULL(found_event);
  CU_ASSERT_PTR_EQUAL(found_event,test_event_one);

  found_event = eventListFind(test_list, "Can not find me");
  CU_ASSERT_PTR_NULL(found_event);
}

void testEventListFindEmptyList() {
  struct EventList *test_list;
  struct Event *found_event;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  found_event = eventListFind(test_list, "sadfsa");
  CU_ASSERT_PTR_NULL(found_event);
}

void testEventListDelete() {
  struct EventList *test_list;
  struct Event *test_event_one, *test_event_two, *test_event_three,
      *test_event_four;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  eventCreate(&test_event_one, "2010-05-24", "06:15", 10, "Event 1", NULL);
  eventCreate(&test_event_two, "2011-05-24", "06:15", 10, "Event 2", NULL);
  eventCreate(&test_event_three, "2012-05-24", "06:15", 10, "Event 3", NULL);
  eventCreate(&test_event_four, "2011-05-24", "16:15", 10, "Event 4", NULL);

  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_one));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_two));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_three));
  CU_ASSERT_TRUE(eventListInsertLast(test_list, test_event_four));

  /* Delete the first one. */
  CU_ASSERT_TRUE(eventListDelete(test_list, test_event_one));

  /* Searching for it in the list, should not fail. */
  CU_ASSERT_PTR_NULL(eventListFind(test_list, "Event 1"));

  /* Delete the one that is now in the middle. */
  CU_ASSERT_TRUE(eventListDelete(test_list, test_event_three));

  /* Searching for it in the list, should not fail. */
  CU_ASSERT_PTR_NULL(eventListFind(test_list, "Event 3"));

  /* Delete the one that's at the end. */
  CU_ASSERT_TRUE(eventListDelete(test_list, test_event_four));

  /* Searching for it in the list, should not fail. */
  CU_ASSERT_PTR_NULL(eventListFind(test_list, "Event 4"));

  /* Delete the remaining one */
  CU_ASSERT_TRUE(eventListDelete(test_list, test_event_two));

  /* Searching for it in the list, should not fail. */
  CU_ASSERT_PTR_NULL(eventListFind(test_list, "Event 2"));

  CU_ASSERT_PTR_NULL(test_list->head);
  CU_ASSERT_PTR_NULL(test_list->tail);

}
