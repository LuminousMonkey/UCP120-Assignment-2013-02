/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#include <CUnit/CUnit.h>

#include "event_list_test.h"

void test_event_list_create_list() {
  struct EventList *test_list;

  test_list = event_list_create();

  if (test_list != NULL) {
    CU_ASSERT_PTR_NULL(test_list->head);
    CU_ASSERT_PTR_NULL(test_list->tail);
    CU_ASSERT_PTR_NULL(test_list->current);

    event_list_destroy(test_list);
  }
}

void test_event_list_insert_single() {
  struct EventList *test_list;
  struct Event *test_event_one;

  test_list = event_list_create();

  if (test_list != NULL) {
    event_create(&test_event_one, "2010-05-24", "07:15", 10, "Event 1", NULL);

    event_list_insert_last(test_list, test_event_one);

    CU_ASSERT_PTR_NOT_NULL(test_list->head);
    CU_ASSERT_PTR_NOT_NULL(test_list->tail);
    CU_ASSERT_PTR_NULL(test_list->current);

    CU_ASSERT_PTR_EQUAL(test_list->head, test_list->tail);

    event_list_destroy(test_list);
  }
}

void test_event_list_insert_multiple() {
  struct EventList *test_list;
  struct Event *test_event_one, *test_event_two, *test_event_three;

  test_list = event_list_create();

  if (test_list != NULL) {
    struct Event *test_event;
    event_create(&test_event_one, "2010-05-24", "06:15", 10, "Event 1", NULL);
    event_create(&test_event_two, "2011-05-24", "06:15", 10, "Event 2", NULL);
    event_create(&test_event_three, "2012-05-24", "06:15", 10, "Event 3", NULL);

    CU_ASSERT_TRUE(event_list_insert_last(test_list, test_event_one));
    CU_ASSERT_TRUE(event_list_insert_last(test_list, test_event_two));
    CU_ASSERT_TRUE(event_list_insert_last(test_list, test_event_three));

    event_list_reset_position(test_list);
    test_event = event_list_next(test_list);

    CU_ASSERT_PTR_EQUAL(test_event_one, test_event);

    test_event = event_list_next(test_list);
    CU_ASSERT_PTR_EQUAL(test_event_two, test_event);

    test_event = event_list_next(test_list);
    CU_ASSERT_PTR_EQUAL(test_event_three, test_event);

    event_list_destroy(test_list);
  }
}
