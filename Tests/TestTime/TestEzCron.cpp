#include <TestEzCron.h>
#include <ezlibs/ezCron.hpp>
#include <ezlibs/ezTime.hpp>
#include <ezlibs/ezCTest.hpp>

#include <iostream>
#include <string>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#define EZ_LOG_IMPLEMENTATION
#include <ezlibs/ezLog.hpp>

using namespace ez::time;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

namespace ez {
namespace time {

class TestCron : public Cron {
public:
    TestCron(const std::string &vRule) : Cron(vRule) { LogVarError("%s", getErrorMessage().c_str()); }
};

}  // namespace time
}  // namespace ez

time_t getEpochTime(int32_t vMin, int32_t vHour, int32_t vMonthDay, int32_t vMonth) {
    struct tm time_info{};
    time_info.tm_year = 2024 - 1900;  // Année 2024
    time_info.tm_mon = vMonth;        // Mois d'octobre (tm_mon commence à 0)
    time_info.tm_mday = vMonthDay;    // 22 octobre
    time_info.tm_hour = vHour;        // 18h
    time_info.tm_min = vMin;          // 00 minutes
    time_info.tm_sec = 25;            // 00 secondes
    time_info.tm_isdst = -1;          // laisse le systeme calculer automatiquement l'heure ete
    return std::mktime(&time_info);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzCron_Format_Mins() {
    // not valid
    TestCron cr("60 * * * *");
    CTEST_ASSERT(!cr.isOk());
    CTEST_ASSERT(cr.getCronRule() == "60 * * * *");
    CTEST_ASSERT(cr.getErrorFlags() == Cron::INVALID_MINUTE);
    return true;
}

bool TestEzCron_Format_Hours() {
    // not valid
    TestCron cr("0 24 * * *");
    CTEST_ASSERT(!cr.isOk());
    CTEST_ASSERT(cr.getCronRule() == "0 24 * * *");
    CTEST_ASSERT(cr.getErrorFlags() == Cron::INVALID_HOUR);
    return true;
}

bool TestEzCron_Format_MonthDay() {
    // not valid
    TestCron cr("0 * 32 * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != Cron::INVALID_MONTH_DAY)
        return false;
    return true;
}

bool TestEzCron_Format_Month() {
    // not valid
    TestCron cr("0 * * 13 *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != Cron::INVALID_MONTH)
        return false;
    return true;
}

bool TestEzCron_Format_WeekDay() {
    // not valid
    TestCron cr("0 * * * 8");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != Cron::INVALID_WEEK_DAY)
        return false;
    return true;
}

bool TestEzCron_Format_ManyErrors_0() {
    TestCron cr("60 24 32 13 8");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() !=
        (Cron::INVALID_MINUTE |     //
         Cron::INVALID_HOUR |       //
         Cron::INVALID_MONTH_DAY |  //
         Cron::INVALID_MONTH |      //
         Cron::INVALID_WEEK_DAY))
        return false;
    return true;
}

bool TestEzCron_Format_ManyErrors_1() {
    TestCron cr("62 28 35 32 68 12");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() !=
        (Cron::INVALID_MINUTE |     //
         Cron::INVALID_HOUR |       //
         Cron::INVALID_MONTH_DAY |  //
         Cron::INVALID_MONTH |      //
         Cron::INVALID_WEEK_DAY |   //
         Cron::INVALID_FIELDS_COUNT))
        return false;
    return true;
}

bool TestEzCron_Format_FieldCount_0() {
    TestCron cr("0 0 1");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != Cron::INVALID_FIELDS_COUNT)
        return false;
    return true;
}

bool TestEzCron_Format_FieldCount_1() {
    TestCron cr("* * * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != Cron::INVALID_FIELDS_COUNT)
        return false;
    return true;
}

bool TestEzCron_Format_Type_Value() {
    // valids
    auto pat = "05 003 0009 005 0004";
    TestCron cr(pat);
    if (!cr.isOk())
        return false;
    auto fields = cr.getFields();
    if (fields.size() != 5)
        return false;
    if (fields.at(0).value != 5)
        return false;
    if (fields.at(1).value != 3)
        return false;
    if (fields.at(2).value != 9)
        return false;
    if (fields.at(3).value != 5)
        return false;
    if (fields.at(4).value != 4)
        return false;
    // not valids
    cr = TestCron("@ _5 $9 ù9 %5");
    if (cr.getErrorFlags() !=
        (Cron::INVALID_MINUTE |     //
         Cron::INVALID_HOUR |       //
         Cron::INVALID_MONTH_DAY |  //
         Cron::INVALID_MONTH |      //
         Cron::INVALID_WEEK_DAY |   //
         Cron::INVALID_CHAR))
        return false;
    if (cr.isOk())
        return false;
    return true;
}

bool TestEzCron_Format_Type_Interval() {
    // valids
    auto cr = TestCron("*/0 */5 */5 */5 */5");
    if (!cr.isOk())
        return false;
    cr = TestCron("*/10 */5 */5 */5 */5");
    if (!cr.isOk())
        return false;
    cr = TestCron("*/45 */5 */5 */5 */5");
    if (!cr.isOk())
        return false;
    cr = TestCron("*/59 */5 */5 */5 */5");
    if (!cr.isOk())
        return false;
    // not valids
    const auto error_flag = (Cron::INVALID_MINUTE |     //
                             Cron::INVALID_HOUR |       //
                             Cron::INVALID_MONTH_DAY |  //
                             Cron::INVALID_MONTH |      //
                             Cron::INVALID_WEEK_DAY |   //
                             Cron::INVALID_INTERVAL);
    cr = TestCron("*5 *5 *5 *5 *5");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != error_flag)
        return false;
    cr = TestCron("*//5 *//5 *//5 *//5 *//5");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != error_flag)
        return false;
    cr = TestCron("*/ */ */ */ */");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != error_flag)
        return false;
    cr = TestCron("*/* */* */* */* */*");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != error_flag)
        return false;
    cr = TestCron("** ** ** ** **");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != error_flag)
        return false;
    return true;
}

bool TestEzCron_Format_Type_Range() {
    // valids
    auto cr = TestCron("0-9 * * * *");
    if (!cr.isOk())
        return false;
    // not valids
    cr = TestCron("5-9-9 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("- * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("-5 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("5-5 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("5--9 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("5-4 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("5- * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE))
        return false;
    cr = TestCron("5-9£9 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_RANGE | Cron::INVALID_CHAR))
        return false;
    return true;
}

bool TestEzCron_Format_Type_Values() {
    // valids
    auto cr = TestCron("0,9,5,6 * * * *");
    if (!cr.isOk())
        return false;
    // not valids
    cr = TestCron(", * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_VALUES))
        return false;
    cr = TestCron("0,2, * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_VALUES))
        return false;
    cr = TestCron(",0 * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_VALUES))
        return false;
    cr = TestCron(",0, * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_VALUES))
        return false;
    cr = TestCron("0,5,69. * * * *");
    if (cr.isOk())
        return false;
    if (cr.getErrorFlags() != (Cron::INVALID_MINUTE | Cron::INVALID_VALUES | Cron::INVALID_CHAR))
        return false;
    return true;
}

bool TestEzCron_TimeCheck_Type_Value() {
    auto cr = TestCron("18 0 * * 2");
    if (!cr.isOk())
        return false;
    // valid value
    if (!cr.isTimeToAct(getEpochTime(18, 0, 22, 9)))
        return false;
    // not valid value
    if (cr.isTimeToAct(getEpochTime(17, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(19, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct())
        return false;
    return true;
}

bool TestEzCron_TimeCheck_Type_Interval() {
    auto cr = TestCron("*/5 0 * * 2");
    if (!cr.isOk())
        return false;
    // valid interval
    if (!cr.isTimeToAct(getEpochTime(0, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(5, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(10, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(15, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(55, 0, 22, 9)))
        return false;
    // not valid interval
    if (cr.isTimeToAct(getEpochTime(1, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(2, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(11, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(54, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(58, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct())
        return false;
    return true;
}

bool TestEzCron_TimeCheck_Type_Range() {
    auto cr = TestCron("15-19 0 * * 2");
    if (!cr.isOk())
        return false;
    // valid range
    if (!cr.isTimeToAct(getEpochTime(15, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(16, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(17, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(18, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(19, 0, 22, 9)))
        return false;
    // not valid ranges
    if (cr.isTimeToAct(getEpochTime(10, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(14, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(20, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(22, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct())
        return false;
    return true;
}

bool TestEzCron_TimeCheck_Type_Values() {
    auto cr = TestCron("10,15,22 0 * * 2");
    if (!cr.isOk())
        return false;
    // valid values
    if (!cr.isTimeToAct(getEpochTime(10, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(15, 0, 22, 9)))
        return false;
    if (!cr.isTimeToAct(getEpochTime(22, 0, 22, 9)))
        return false;
    // not valid values
    if (cr.isTimeToAct(getEpochTime(5, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(9, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(11, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(16, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct(getEpochTime(23, 0, 22, 9)))
        return false;
    if (cr.isTimeToAct())
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzCron(const std::string &vTest) {
    IfTestExist(TestEzCron_Format_Mins);
    else IfTestExist(TestEzCron_Format_Hours);
    else IfTestExist(TestEzCron_Format_MonthDay);
    else IfTestExist(TestEzCron_Format_Month);
    else IfTestExist(TestEzCron_Format_WeekDay);
    else IfTestExist(TestEzCron_Format_ManyErrors_0);
    else IfTestExist(TestEzCron_Format_ManyErrors_1);
    else IfTestExist(TestEzCron_Format_FieldCount_0);
    else IfTestExist(TestEzCron_Format_FieldCount_1);
    else IfTestExist(TestEzCron_Format_Type_Value);
    else IfTestExist(TestEzCron_Format_Type_Interval);
    else IfTestExist(TestEzCron_Format_Type_Range);
    else IfTestExist(TestEzCron_Format_Type_Values);
    else IfTestExist(TestEzCron_TimeCheck_Type_Value);
    else IfTestExist(TestEzCron_TimeCheck_Type_Interval);
    else IfTestExist(TestEzCron_TimeCheck_Type_Range);
    else IfTestExist(TestEzCron_TimeCheck_Type_Values);
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
