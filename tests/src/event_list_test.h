/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 *
 * Unit tests for the EventList
 */

#ifndef EVENT_LIST_TEST_H_
#define EVENT_LIST_TEST_H_

#include "event_list.h"

/* Create an empty list */
void test_event_list_create_list();

/* Insert a single item into the list, and that it's the head and tail */
void test_event_list_insert_single();

/* Insert a few items, and test the order. */
/* void test_event_list_insert_multiple(); */

#endif
