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
  }
}
