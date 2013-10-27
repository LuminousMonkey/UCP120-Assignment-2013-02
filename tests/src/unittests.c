#include <CUnit/Basic.h>

#include "date_test.h"

int main() {
  CU_pSuite pSuite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  pSuite = CU_add_suite("Suite_1", NULL, NULL);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "Test Parse Valid Date", test_date_parse_valid_date)) ||
      (NULL == CU_add_test(pSuite, "Test Parse No of fields Date", test_date_parse_invalid_no_fields)) ||
      (NULL == CU_add_test(pSuite, "Test Parse Feb on leap year", test_date_parse_feb_on_leap))) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
