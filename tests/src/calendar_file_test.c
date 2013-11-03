/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#include <CUnit/Cunit.h>

#include "calendar_file_test.h"
#include "calendar_file.h"
#include "event_list.h"

void testCalendarLoadFile() {
  struct EventList *test_list;
  enum FileError error_result;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  error_result = loadCalendar(test_list, "data/test.txt");
  CU_ASSERT_EQUAL(FILE_NO_ERROR, error_result);

  eventListDestroy(test_list);

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  error_result = loadCalendar(test_list, "data/eof-on-name.txt");
  CU_ASSERT_EQUAL(FILE_NO_ERROR, error_result);

  CU_ASSERT_PTR_NOT_NULL(test_list->head);
}

void testCalendarLoadInvalidFiles() {
  struct EventList *test_list;
  enum FileError error_result;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  error_result = loadCalendar(test_list, "data/eof-too-early.txt");
  CU_ASSERT_EQUAL(FILE_INVALID_FORMAT, error_result);


}

void testCalendarSaveFile() {
  struct EventList *test_list;
  enum FileError error_result;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  error_result = loadCalendar(test_list, "data/test.txt");
  CU_ASSERT_EQUAL(FILE_NO_ERROR, error_result);

  error_result = saveCalendar(test_list, "saved/test.txt");
  CU_ASSERT_EQUAL(FILE_NO_ERROR, error_result);
}

void testCalendarSaveOverDir() {
  struct EventList *test_list;
  enum FileError error_result;

  test_list = eventListCreate();
  CU_ASSERT_PTR_NOT_NULL(test_list);

  error_result = loadCalendar(test_list, "data/test.txt");
  CU_ASSERT_EQUAL(FILE_NO_ERROR, error_result);

  error_result = saveCalendar(test_list, "data");
  CU_ASSERT_EQUAL(FILE_ERROR, error_result);

  CU_ASSERT_PTR_NOT_NULL(test_list->head);
}
