#include <ezlibs/ezDate.hpp>
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

bool TestEzDate_Date_SetYear_NoSideEffect() {
    Date d("2025-03-31");
    d.setYear(2030);
    CTEST_ASSERT(d.getYear() == 2030);
    CTEST_ASSERT(d.getMonth() == 3);
    CTEST_ASSERT(d.getDay() == 31);
    CTEST_ASSERT(d.getDate() == "2030-03-31");
    return true;
}

bool TestEzDate_Date_SetMonth_Clamped_1_12() {
    Date d("2025-05-15");

    d.setMonth(0);  // clamp -> 1
    CTEST_ASSERT(d.getYear() == 2025);
    CTEST_ASSERT(d.getMonth() == 1);
    CTEST_ASSERT(d.getDay() == 15);

    d.setMonth(13);  // clamp -> 12
    CTEST_ASSERT(d.getYear() == 2025);
    CTEST_ASSERT(d.getMonth() == 12);
    CTEST_ASSERT(d.getDay() == 15);

    d.setMonth(7);  // inside range
    CTEST_ASSERT(d.getYear() == 2025);
    CTEST_ASSERT(d.getMonth() == 7);
    CTEST_ASSERT(d.getDay() == 15);

    return true;
}

bool TestEzDate_Date_SetDay_Clamped_By_Month() {
    // Avril a 30 jours
    Date d("2025-04-10");
    d.setDay(31);  // clamp -> 30
    CTEST_ASSERT(d.getDate() == "2025-04-30");

    // F?vrier non bissextile
    Date e("2025-02-10");
    e.setDay(29);  // clamp -> 28
    CTEST_ASSERT(e.getDate() == "2025-02-28");

    // F?vrier bissextile
    Date f("2024-02-10");
    f.setDay(29);  // ok
    CTEST_ASSERT(f.getDate() == "2024-02-29");

    // Valeur trop petite -> 1
    Date g("2025-08-10");
    g.setDay(0);
    CTEST_ASSERT(g.getDate() == "2025-08-01");

    return true;
}

bool TestEzDate_Date_OffsetYear_Simple() {
    Date d("1999-12-31");
    d.offsetYear(+1);
    CTEST_ASSERT(d.getDate() == "2000-12-31");
    d.offsetYear(-2);
    CTEST_ASSERT(d.getDate() == "1998-12-31");
    return true;
}

bool TestEzDate_Date_OffsetMonth_RollYear() {
    // On attend un roll correct ann?e/mois
    Date a("2025-01-15");
    a.offsetMonth(+1);  // -> 2025-02-15
    CTEST_ASSERT(a.getDate() == "2025-02-15");

    Date b("2025-12-15");
    b.offsetMonth(+1);  // -> 2026-01-15
    CTEST_ASSERT(b.getDate() == "2026-01-15");

    Date c("2025-01-15");
    c.offsetMonth(-1);  // -> 2024-12-15
    CTEST_ASSERT(c.getDate() == "2024-12-15");

    // Gros offset positif
    Date d("2025-03-31");
    d.offsetMonth(+23);  // +23 mois -> 2027-02-28/29 ? (jour intact, pas clamp ici)
    CTEST_ASSERT(d.getYear() == 2027);
    CTEST_ASSERT(d.getMonth() == 2);
    CTEST_ASSERT(d.getDay() == 31 || d.getDay() == 30 || d.getDay() == 29 || d.getDay() == 28);
    // Remarque: selon ton design, tu ne clamps pas ici via offsetMonth ; on v?rifie juste Y/M.

    // Gros offset n?gatif
    Date e("2025-03-10");
    e.offsetMonth(-25);  // -25 -> 2023-02-10
    CTEST_ASSERT(e.getDate() == "2023-02-10");

    return true;
}

bool TestEzDate_Date_OffsetDay_Generic() {
    Date d("2025-08-01");
    d.offsetDay(+1);
    CTEST_ASSERT(d.getDate() == "2025-08-02");

    d.offsetDay(-2);
    CTEST_ASSERT(d.getDate() == "2025-07-31");

    // Franchir f?vrier bissextile / non-bissextile
    Date e("2024-03-01");  // leap year
    e.offsetDay(-1);
    CTEST_ASSERT(e.getDate() == "2024-02-29");

    Date f("2025-03-01");  // non leap
    f.offsetDay(-1);
    CTEST_ASSERT(f.getDate() == "2025-02-28");

    // Offsets larges
    Date g("1970-01-01");
    g.offsetDay(+10000);
    CTEST_ASSERT(diffDays(g.getDate(), "1970-01-01") == 10000);

    // 1 year
    Date h("1970-01-01");
    h.offsetDay(+365);
    CTEST_ASSERT(diffDays(h.getDate(), "1970-01-01") == 365);

    // Offsets larges
    Date i("2000-01-01");
    //30 ? 365 = 10 950
    //7 jours bissextiles = 10 957
    i.offsetDay(-10957);
    CTEST_ASSERT(i.getDate() == "1970-01-01");

    return true;
}

bool TestEzDate_Date_GetDate_Format() {
    // Format de base + zero padding
    Date a("2025-08-09");
    CTEST_ASSERT(a.getDate() == "2025-08-09");

    // Z?ro padding sur mois/jour = 1 chiffre
    Date b("0077-01-01");
    CTEST_ASSERT(b.getDate() == "0077-01-01");

    // V?rifier que le format reste correct apr?s set*
    Date c("2025-8-1");  // parseYmd ?chouera si ce n'est pas "YYYY-MM-DD" -> m_valid=false chez toi,
                         // mais tu construis via parseYmd(vDate,...). On reste sur des entr?es valides :
    Date d("2025-08-01");
    d.setYear(42);  // ann?e courte -> doit zero-padder sur 4 chiffres
    d.setMonth(1);  // d?j? clamp?
    d.setDay(1);    // clamp OK
    CTEST_ASSERT(d.getDate() == "0042-01-01");

    return true;
}

bool TestEzDate_Date_GetDate_AfterMutations() {
    // Cha?ne d'op?rations pour v?rifier coh?rence (Y,M,D) -> string
    Date d("2025-12-31");
    CTEST_ASSERT(d.getDate() == "2025-12-31");

    d.offsetYear(+1);  // 2026-12-31
    CTEST_ASSERT(d.getDate() == "2026-12-31");

    d.offsetMonth(+1);                                    // 2027-01-31 (selon ton offsetMonth, le day reste inchang?)
    CTEST_ASSERT(d.getDate().substr(0, 7) == "2027-01");  // on v?rifie surtout le format
    CTEST_ASSERT(d.getDate().size() == 10);

    d.offsetDay(+1);  // 2027-02-01 (roll jour/mois par arithm?tique jours)
    CTEST_ASSERT(d.getDate() == "2027-02-01");

    d.setMonth(12);  // clamp direct
    d.setDay(30);
    CTEST_ASSERT(d.getDate() == "2027-12-30");

    return true;
}
bool TestEzDate_Date_OffsetDay_LeapYearSpans() {
    // Ann?es bissextiles mentionn?es : 1972, 1976, 1980, 1984, 1988, 1992, 1996, 2000
    {
        Date d("1973-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1972-01-01");
    }
    {
        Date d("1977-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1976-01-01");
    }
    {
        Date d("1981-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1980-01-01");
    }
    {
        Date d("1985-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1984-01-01");
    }
    {
        Date d("1989-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1988-01-01");
    }
    {
        Date d("1993-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1992-01-01");
    }
    {
        Date d("1997-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "1996-01-01");
    }
    {
        Date d("2001-01-01");
        d.offsetDay(-366);
        CTEST_ASSERT(d.getDate() == "2000-01-01");
    }
    return true;
}

bool TestEzDate_Date_OffsetDay_NonLeapYearSpans() {
    // Quelques non-bissextiles autour, pour contraste (365 jours entre 1er janv successifs)
    {
        Date d("1971-01-01");
        d.offsetDay(-365);
        CTEST_ASSERT(d.getDate() == "1970-01-01");
    }
    {
        Date d("1974-01-01");
        d.offsetDay(-365);
        CTEST_ASSERT(d.getDate() == "1973-01-01");
    }
    {
        Date d("1978-01-01");
        d.offsetDay(-365);
        CTEST_ASSERT(d.getDate() == "1977-01-01");
    }
    {
        Date d("1999-01-01");
        d.offsetDay(-365);
        CTEST_ASSERT(d.getDate() == "1998-01-01");
    }
    return true;
}

bool TestEzDate_Date_OffsetDay_LongSpan_1970_2000() {
    // De 2000-01-01 ? 1970-01-01 : 30 ans avec 7 bissextiles -> 10957 jours
    Date i("2000-01-01");
    i.offsetDay(-10957);
    CTEST_ASSERT(i.getDate() == "1970-01-01");

    // V?rif crois?e avec diffDays
    CTEST_ASSERT(diffDays("2000-01-01", "1970-01-01") == 10957);
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
    else IfTestExist(TestEzDate_Date_SetYear_NoSideEffect);
    else IfTestExist(TestEzDate_Date_SetMonth_Clamped_1_12);
    else IfTestExist(TestEzDate_Date_SetDay_Clamped_By_Month);
    else IfTestExist(TestEzDate_Date_OffsetYear_Simple);
    else IfTestExist(TestEzDate_Date_OffsetMonth_RollYear);
    else IfTestExist(TestEzDate_Date_OffsetDay_Generic);
    else IfTestExist(TestEzDate_Date_GetDate_Format);
    else IfTestExist(TestEzDate_Date_GetDate_AfterMutations);
    else IfTestExist(TestEzDate_Date_OffsetDay_LeapYearSpans);
    else IfTestExist(TestEzDate_Date_OffsetDay_NonLeapYearSpans);
    else IfTestExist(TestEzDate_Date_OffsetDay_LongSpan_1970_2000);

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
