#include <CUnit/Basic.h>

#include "date_test.h"
#include "event_list_test.h"
#include "event_test.h"

int main() {
  CU_pSuite pDateSuite = NULL;
  CU_pSuite pTimeSuite = NULL;
  CU_pSuite pEventSuite = NULL;
  CU_pSuite pEventListSuite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  pDateSuite = CU_add_suite("Date Checks", NULL, NULL);
  pTimeSuite = CU_add_suite("Time Checks", NULL, NULL);
  pEventSuite = CU_add_suite("Event Checks", NULL, NULL);
  pEventListSuite = CU_add_suite("Event List Checks", NULL, NULL);

  if ((NULL == pDateSuite) || (NULL == pTimeSuite) ||
      (NULL == pEventSuite) || (NULL == pEventListSuite)) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pDateSuite, "Test Parse Valid Date",
                           test_date_parse_valid_date)) ||
      (NULL == CU_add_test(pDateSuite, "Test Parse No of fields Date",
                           test_date_parse_invalid_no_fields)) ||
      (NULL == CU_add_test(pDateSuite, "Test Parse Feb on leap year",
                           test_date_parse_feb_on_leap)) ||
      (NULL == CU_add_test(pDateSuite, "Test Parse Feb on non leap year",
                           test_date_parse_feb_on_non_leap)) ||
      (NULL == CU_add_test(pDateSuite, "Test error codes correct",
                           test_date_parse_correct_errors)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Parse Valid Time",
                           test_time_parse_valid_time)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Parse Invalid Time String",
                           test_time_parse_invalid_time)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Time Over Max Hours",
                           test_time_over_max_hours)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Time No Minutes Over Max",
                           test_time_no_minutes_over_max)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Valid Event",
                           test_create_event)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Duration",
                           test_create_event_invalid_duration)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Time",
                           test_create_event_invalid_date)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Date",
                           test_create_event_invalid_time)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Name",
                           test_create_event_invalid_name)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event No Location",
                           test_create_event_no_location)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Create Empty Event List",
                           test_event_list_create_list)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Even List Insert Single",
                           test_event_list_insert_single)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Even List Insert Multiple",
                           test_event_list_insert_multiple))

      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
