#include <ezlibs/EzDate.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

// Disable noisy conversion warnings to match your style
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

using namespace ez::date;

bool TestEzDate_ParseYmd() {
    int y=0,m=0,d=0;
    CTEST_ASSERT(parseYmd("2025-08-01", y, m, d) == true);
    CTEST_ASSERT(y == 2025 && m == 8 && d == 1);

    CTEST_ASSERT(parseYmd("1970-01-01", y, m, d) == true);
    CTEST_ASSERT(y == 1970 && m == 1 && d == 1);

    CTEST_ASSERT(parseYmd("2025-13-01", y, m, d) == false);
    CTEST_ASSERT(parseYmd("2025-00-10", y, m, d) == false);
    CTEST_ASSERT(parseYmd("2025-08-00", y, m, d) == false);
    CTEST_ASSERT(parseYmd("bad", y, m, d) == false);
    return true;
}

bool TestEzDate_IsLeap_MonthDays() {
    CTEST_ASSERT(isLeap(2024) == true);
    CTEST_ASSERT(isLeap(2025) == false);
    CTEST_ASSERT(isLeap(2000) == true);
    CTEST_ASSERT(isLeap(2100) == false);

    CTEST_ASSERT(monthDays(2024, 2) == 29);
    CTEST_ASSERT(monthDays(2025, 2) == 28);
    CTEST_ASSERT(monthDays(2025, 1) == 31);
    CTEST_ASSERT(monthDays(2025, 4) == 30);
    return true;
}

bool TestEzDate_AddDays_Generic() {
    // Simple move forward/backward
    CTEST_ASSERT(addDays("2025-08-01", 0) == "2025-08-01");
    CTEST_ASSERT(addDays("2025-08-01", 1) == "2025-08-02");
    CTEST_ASSERT(addDays("2025-08-01", -1) == "2025-07-31");

    // Across months
    CTEST_ASSERT(addDays("2025-08-01", -2) == "2025-07-30");
    CTEST_ASSERT(addDays("2025-01-31", 1)  == "2025-02-01");

    // Across leap February
    CTEST_ASSERT(addDays("2024-03-01", -1) == "2024-02-29"); // leap
    CTEST_ASSERT(addDays("2025-03-01", -1) == "2025-02-28"); // non-leap
    return true;
}

bool TestEzDate_DiffDays() {
    CTEST_ASSERT(diffDays("1970-01-02", "1970-01-01") == 1);
    CTEST_ASSERT(diffDays("1970-01-01", "1970-01-02") == -1);
    CTEST_ASSERT(diffDays("2025-08-10", "2025-08-10") == 0);

    // Cross-month distances
    CTEST_ASSERT(diffDays("2025-03-01", "2025-02-28") == 1);
    // Leap-aware
    CTEST_ASSERT(diffDays("2024-03-01", "2024-02-29") == 1);
    return true;
}

bool TestEzDate_Start_End_Of_Month() {
    CTEST_ASSERT(startOfMonth("2025-08-10") == "2025-08-01");
    CTEST_ASSERT(endOfMonth  ("2025-08-10") == "2025-08-31");

    // February non-leap
    CTEST_ASSERT(endOfMonth("2025-02-10") == "2025-02-28");
    // February leap
    CTEST_ASSERT(endOfMonth("2024-02-10") == "2024-02-29");
    return true;
}

bool TestEzDate_YmKey() {
    CTEST_ASSERT(ymKey("2025-08-10") == "2025-08");
    CTEST_ASSERT(ymKey("2025-08")    == "2025-08");
    CTEST_ASSERT(ymKey("2025")       == "2025");   // passthrough (shorter than 7 chars)
    return true;
}

bool TestEzDate_DayOfMonthToDate_PositiveNegative() {
    const std::string firstAug = "2025-08-01";

    // day >= 1 => day-of-month relative to first
    CTEST_ASSERT(dayOfMonthToDate(1, firstAug) == "2025-08-01");
    CTEST_ASSERT(dayOfMonthToDate(2, firstAug) == "2025-08-02");
    CTEST_ASSERT(dayOfMonthToDate(31, firstAug) == "2025-08-31"); // relies on month length, OK for August

    // day < 1 => negative offset from the 1st
    CTEST_ASSERT(dayOfMonthToDate(-1, firstAug) == "2025-07-31");
    CTEST_ASSERT(dayOfMonthToDate(-2, firstAug) == "2025-07-30");

    // day == 0 => invalid in the 1..31 model; current policy returns firstOfMonth
    CTEST_ASSERT(dayOfMonthToDate(0, firstAug) == "2025-08-01");
    return true;
}

bool TestEzDate_DayOfMonthToDate_FebruaryLeap() {
    const std::string firstMarLeap    = "2024-03-01"; // 2024 is leap
    const std::string firstMarNonLeap = "2025-03-01";

    // Negative offsets crossing into February (leap vs non-leap)
    CTEST_ASSERT(dayOfMonthToDate(-1, firstMarLeap)    == "2024-02-29");
    CTEST_ASSERT(dayOfMonthToDate(-2, firstMarLeap)    == "2024-02-28");
    CTEST_ASSERT(dayOfMonthToDate(-1, firstMarNonLeap) == "2025-02-28");
    CTEST_ASSERT(dayOfMonthToDate(-2, firstMarNonLeap) == "2025-02-27");

    // Positive day-of-month from the 1st works normally
    CTEST_ASSERT(dayOfMonthToDate(1, firstMarLeap) == "2024-03-01");
    CTEST_ASSERT(dayOfMonthToDate(2, firstMarLeap) == "2024-03-02");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzDate(const std::string& vTest) {
    IfTestExist(TestEzDate_ParseYmd);
    else IfTestExist(TestEzDate_IsLeap_MonthDays);
    else IfTestExist(TestEzDate_AddDays_Generic);
    else IfTestExist(TestEzDate_DiffDays);
    else IfTestExist(TestEzDate_Start_End_Of_Month);
    else IfTestExist(TestEzDate_YmKey);
    else IfTestExist(TestEzDate_DayOfMonthToDate_PositiveNegative);
    else IfTestExist(TestEzDate_DayOfMonthToDate_FebruaryLeap);
    return false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
