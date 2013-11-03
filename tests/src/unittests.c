#include <CUnit/Basic.h>

#include "calendar_file_test.h"
#include "date_test.h"
#include "event_list_test.h"
#include "event_test.h"

int main() {
  CU_pSuite pDateSuite = NULL;
  CU_pSuite pTimeSuite = NULL;
  CU_pSuite pEventSuite = NULL;
  CU_pSuite pEventListSuite = NULL;
  CU_pSuite pCalendarFileSuite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  pDateSuite = CU_add_suite("Date Checks", NULL, NULL);
  pTimeSuite = CU_add_suite("Time Checks", NULL, NULL);
  pEventSuite = CU_add_suite("Event Checks", NULL, NULL);
  pEventListSuite = CU_add_suite("Event List Checks", NULL, NULL);
  pCalendarFileSuite = CU_add_suite("Calendar File Checks", NULL, NULL);

  if ((NULL == pDateSuite) || (NULL == pTimeSuite) ||
      (NULL == pEventSuite) || (NULL == pEventListSuite) ||
      (NULL == pCalendarFileSuite)) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pDateSuite, "Test Parse Valid Date",
                           testDateParseValidDate)) ||
      (NULL == CU_add_test(pDateSuite, "Test Parse No of fields Date",
                           testDateParseInvalidNoFields)) ||
      (NULL == CU_add_test(pDateSuite, "Test Parse Feb on leap year",
                           testDateParseFebOnLeap)) ||
      (NULL == CU_add_test(pDateSuite, "Test Parse Feb on non leap year",
                           testDateParseFebOnNonLeap)) ||
      (NULL == CU_add_test(pDateSuite, "Test error codes correct",
                           testDateParseCorrectErrors)) ||
      (NULL == CU_add_test(pDateSuite, "Test Date String Output",
                           testDateStringOutput)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Parse Valid Time",
                           testTimeParseValidTime)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Parse Invalid Time String",
                           testTimeParseInvalidTime)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Time Over Max Hours",
                           testTimeOverMaxHours)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Time No Minutes Over Max",
                           testTimeNoMinutesOverMax)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Time String Output",
                           testTimeStringOutput)) ||
      (NULL == CU_add_test(pTimeSuite, "Test Duration String Output",
                           testDurationStringOutput)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Valid Event",
                           testCreateEvent)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Duration",
                           testCreateEventInvalidDuration)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Time",
                           testCreateEventInvalidDate)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Date",
                           testCreateEventInvalidTime)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event Invalid Name",
                           testCreateEventInvalidName)) ||
      (NULL == CU_add_test(pEventSuite, "Test Create Event No Location",
                           testCreateEventNoLocation)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Create Empty Event List",
                           testEventListCreateList)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Event List Insert Single",
                           testEventListInsertSingle)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Event List Insert Null",
                           testEventListInsertNull)) ||
      (NULL == CU_add_test(pEventListSuite, "Test Even List Insert Multiple",
                           testEventListInsertMultiple)) ||
      (NULL == CU_add_test(pCalendarFileSuite, "Test Load Calendar File",
                           testCalendarLoadFile)) ||
      (NULL == CU_add_test(pCalendarFileSuite, "Test Load Invalid Calendar Files",
                           testCalendarLoadInvalidFiles)) ||
      (NULL == CU_add_test(pCalendarFileSuite, "Test Save Calendar File",
                           testCalendarSaveFile)) ||
      (NULL == CU_add_test(pCalendarFileSuite, "Test Save Calendar Over Dir",
                           testCalendarSaveOverDir))

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
