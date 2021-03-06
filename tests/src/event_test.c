/*
 * UCP 120 - Assignment
 *
 * Author: Mike Aldred
 *
 * Unit tests for events.
 *
 * Date and Time have their own tests, so don't do any date/time
 * checking beyond testing that correct error codes are return by the
 * event functions.
 */

#include <string.h>
#include <CUnit/CUnit.h>

#include "event_test.h"

#define VALID_DATE "2010-10-10"
#define VALID_TIME "18:05"
#define VALID_NAME "Blah"
#define VALID_DURATION 75
#define VALID_LOCATION "Office"

/*
 * Forward declarations
 */
static void cleanUpEvent(struct Event **event);

/*
 * Don't test date or time, they have their own tests.
 */
void testCreateEvent() {
  enum EventError error_result;
  struct Event *test_event;

  error_result = eventCreate(&test_event, VALID_DATE, VALID_TIME,
                             73, VALID_NAME, VALID_LOCATION);

  CU_ASSERT_EQUAL(error_result, EVENT_NO_ERROR);
  CU_ASSERT_PTR_NOT_NULL(test_event);

  CU_ASSERT_EQUAL(test_event->time.hour, 18);
  CU_ASSERT_EQUAL(test_event->time.minutes, 5);

  CU_ASSERT_EQUAL(test_event->date.year, 2010);
  CU_ASSERT_EQUAL(test_event->date.month, 10);
  CU_ASSERT_EQUAL(test_event->date.day, 10);

  CU_ASSERT_EQUAL(0, strcmp(test_event->name, VALID_NAME));
  CU_ASSERT_EQUAL(0, strcmp(test_event->location, VALID_LOCATION));

  /*
   * Test Event destroy as well.
   */
  cleanUpEvent(&test_event);
}

void testCreateEventInvalidDuration() {
  enum EventError error_result;
  struct Event *test_event;

  error_result = eventCreate(&test_event, VALID_DATE, VALID_TIME,
                             -1, VALID_NAME, VALID_LOCATION);

  CU_ASSERT_EQUAL(error_result, EVENT_DURATION_INVALID);
  CU_ASSERT_PTR_NULL(test_event);
}

void testCreateEventInvalidTime() {
  enum EventError error_result;
  struct Event *test_event;

  error_result = eventCreate(&test_event, VALID_DATE, "24:01",
                             VALID_DURATION, VALID_NAME, VALID_LOCATION);

  CU_ASSERT_EQUAL(error_result, EVENT_TIME_INVALID);
  CU_ASSERT_PTR_NULL(test_event);
}

void testCreateEventInvalidDate() {
  enum EventError error_result;
  struct Event *test_event;

  error_result = eventCreate(&test_event, "0-01-01", VALID_TIME,
                             VALID_DURATION, VALID_NAME, VALID_LOCATION);

  CU_ASSERT_EQUAL(error_result, EVENT_DATE_INVALID);
  CU_ASSERT_PTR_NULL(test_event);
}

void testCreateEventInvalidName() {
  enum EventError error_result;
  struct Event *test_event;

  error_result = eventCreate(&test_event, VALID_DATE, VALID_TIME,
                             VALID_DURATION, "", VALID_LOCATION);

  CU_ASSERT_EQUAL(error_result, EVENT_NAME_INVALID);
  CU_ASSERT_PTR_NULL(test_event);

  cleanUpEvent(&test_event);

  error_result = eventCreate(&test_event, VALID_DATE, VALID_TIME,
                             VALID_DURATION, NULL, VALID_LOCATION);

  CU_ASSERT_EQUAL(error_result, EVENT_NAME_INVALID);
  CU_ASSERT_PTR_NULL(test_event);

  cleanUpEvent(&test_event);
}

void testCreateEventNoLocation() {
  enum EventError error_result;
  struct Event *test_event;

  error_result = eventCreate(&test_event, VALID_DATE, VALID_TIME,
                             VALID_DURATION, VALID_NAME, NULL);

  CU_ASSERT_EQUAL(error_result, EVENT_NO_ERROR);
  CU_ASSERT_PTR_NOT_NULL(test_event);

  cleanUpEvent(&test_event);

  /*
   * Should be able to handle empty strings.
   * They should just be treated as NULL locations.
   */
  error_result = eventCreate(&test_event, VALID_DATE, VALID_TIME,
                             VALID_DURATION, VALID_NAME, "");

  CU_ASSERT_EQUAL(error_result, EVENT_NO_ERROR);
  CU_ASSERT_PTR_NOT_NULL(test_event);

  cleanUpEvent(&test_event);
}

static void cleanUpEvent(struct Event **event) {
  eventDestroy(*event);
  *event = NULL;
}
