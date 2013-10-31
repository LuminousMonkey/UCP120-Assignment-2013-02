#ifndef DATE_TEST_H_
#define DATE_TEST_H_

#include "date_time.h"

void testDateParseValidDate();

void testDateParseInvalidNoFields();

void testDateParseFebOnLeap();

void testDateParseFebOnNonLeap();

void testDateParseCorrectErrors();

void testTimeParseValidTime();

void testTimeParseInvalidTime();

void testTimeOverMaxHours();

void testTimeNoMinutesOverMax();

#endif
