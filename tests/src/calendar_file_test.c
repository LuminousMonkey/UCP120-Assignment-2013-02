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

  CU_ASSERT_PTR_NOT_NULL( test_list );

  error_result = loadCalendar( test_list, "data/test.txt" );

  CU_ASSERT_EQUAL( FILE_EOF, error_result );
}
