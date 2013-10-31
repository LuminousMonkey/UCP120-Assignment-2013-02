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

  CU_ASSERT_PTR_NOT_NULL( test_list );

  CU_ASSERT_PTR_NULL( test_list->head );
  CU_ASSERT_PTR_NULL( test_list->tail );
  CU_ASSERT_PTR_NULL( test_list->current );

  eventListDestroy( test_list );
}

void testEventListInsertSingle() {
  struct EventList *test_list;
  struct Event *test_event_one;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL( test_list );

  eventCreate( &test_event_one, "2010-05-24", "07:15", 10, "Event 1", NULL );

  eventListInsertLast( test_list, test_event_one );

  CU_ASSERT_PTR_NOT_NULL( test_list->head );
  CU_ASSERT_PTR_NOT_NULL( test_list->tail );
  CU_ASSERT_PTR_NULL( test_list->current );

  CU_ASSERT_PTR_EQUAL( test_list->head, test_list->tail );

  eventListDestroy( test_list );
}

void testEventListInsertMultiple() {
  struct EventList *test_list;
  struct Event *test_event_one, *test_event_two,
      *test_event_three, *compare_event;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL( test_list );

  eventCreate( &test_event_one, "2010-05-24", "06:15", 10, "Event 1", NULL );
  eventCreate( &test_event_two, "2011-05-24", "06:15", 10, "Event 2", NULL );
  eventCreate( &test_event_three, "2012-05-24", "06:15", 10, "Event 3", NULL );

  CU_ASSERT_TRUE( eventListInsertLast( test_list, test_event_one ) );
  CU_ASSERT_TRUE( eventListInsertLast( test_list, test_event_two ) );
  CU_ASSERT_TRUE( eventListInsertLast( test_list, test_event_three ) );

  eventListResetPosition( test_list );
  compare_event = eventListNext( test_list );

  CU_ASSERT_PTR_EQUAL( test_event_one, compare_event );

  compare_event = eventListNext( test_list );
  CU_ASSERT_PTR_EQUAL( test_event_two, compare_event );

  compare_event = eventListNext( test_list );
  CU_ASSERT_PTR_EQUAL( test_event_three, compare_event );

  eventListDestroy( test_list );
}

/* Inserting a null event should do nothing to the list. */
void testEventListInsertNull() {
  struct EventList *test_list;

  test_list = eventListCreate();

  CU_ASSERT_PTR_NOT_NULL( test_list );

  CU_ASSERT_EQUAL( eventListInsertLast( test_list, NULL ), FALSE );

  /* Just check that the head and tail are still NULL */
  CU_ASSERT_PTR_NULL( test_list->head );
  CU_ASSERT_PTR_NULL( test_list->tail );
}
