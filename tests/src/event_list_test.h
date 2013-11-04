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
void testEventListCreateList();

/* Insert a single item into the list, and that it's the head and tail */
void testEventListInsertSingle();

/* Insert a few items, and test the order. */
void testEventListInsertMultiple();

/* List should not add NULLs */
void testEventListInsertNull();

/* Search that we can search events. */
void testEventListFind();

/* Should get nothing back from and empty list. */
void testEventListFindEmptyList();

#endif
