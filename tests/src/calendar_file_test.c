/*
 * UCP 120 Assignment
 *
 * Author: Mike Aldred
 */

#include <CUnit/Cunit.h>

#include "calendar_file_test.h"
#include "calendar_file.h"
#include "event_list.h"

void test_calendar_load_file() {
  struct EventList *test_list;
  enum FileError error_result;

  test_list = event_list_create();

  CU_ASSERT_PTR_NOT_NULL(test_list);

  error_result = load_calendar(test_list, "data/test.txt");

  CU_ASSERT_EQUAL(FILE_NO_ERROR,error_result);
}
