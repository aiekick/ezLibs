#pragma once

/*
MIT License

Copyright (c) 2014-2025 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezDate is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <string>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <cstdlib>

/**
 * @file EzDate.hpp
 * @brief Simple civil date utilities (YYYY-MM-DD), header-only.
 *
 * Design:
 * - `addDays(date, offset)` performs generic civil date arithmetic using a day count since 1970-01-01.
 * - `dayOfMonthToDate(day, firstOfMonth)` interprets a SQL-like "day index in month":
 *      * day >= 1 => day-of-month (1..31) relative to the 1st
 *      * day <  1 => negative offset from the 1st (e.g., -2 => two days before the 1st)
 *      * day == 0 => invalid in this model (returns firstOfMonth as fallback)
 *
 * Leap years and month lengths are handled by Gregorian rules.
 */

namespace ez {
namespace date {
/**
 * @brief Parse a "YYYY-MM-DD" date string into numeric year, month, and day.
 *
 * @param s Date string to parse.
 * @param y [out] Parsed year.
 * @param m [out] Parsed month in [1..12].
 * @param d [out] Parsed day in [1..31].
 * @return true if parsing succeeds and fields look valid; false otherwise.
 */
inline bool parseYmd(const std::string& s, int32_t& y, int32_t& m, int32_t& d) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-')
        return false;
    y = std::atoi(s.substr(0, 4).c_str());
    m = std::atoi(s.substr(5, 2).c_str());
    d = std::atoi(s.substr(8, 2).c_str());
    return (y != 0 && m >= 1 && m <= 12 && d >= 1 && d <= 31);
}

/**
 * @brief Check if a given year is a leap year (Gregorian rules).
 *
 * @param y Year.
 * @return true if leap year; false otherwise.
 */
inline bool isLeap(int32_t y) {
    return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
}

/**
 * @brief Number of days in a given month of a given year.
 *
 * @param y Year.
 * @param m Month in [1..12].
 * @return Number of days in that month (28, 29, 30, or 31).
 */
inline int32_t monthDays(int32_t y, int32_t m) {
    static const int32_t md[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int32_t d = md[m - 1];
    if (m == 2 && isLeap(y))
        d = 29;
    return d;
}

/**
 * @brief Convert a date to the number of civil days since 1970-01-01.
 *
 * This is a simple civil-day count (not time-zone aware and not tied to Unix time).
 *
 * @param Y Year.
 * @param M Month in [1..12].
 * @param D Day in [1..31].
 * @return Days since 1970-01-01 (can be negative for dates before 1970-01-01).
 */
inline int32_t ymdToDays(int32_t Y, int32_t M, int32_t D) {
    int32_t y = Y, m = M, d = D;
    int32_t n = 0;
    for (int32_t yy = 1970; yy < y; ++yy)
        n += 365 + (isLeap(yy) ? 1 : 0);
    for (int32_t mm = 1; mm < m; ++mm)
        n += monthDays(y, mm);
    n += d - 1;
    return n;
}

/**
 * @brief Convert a civil day count since 1970-01-01 back to a date.
 *
 * @param days Day count since 1970-01-01.
 * @param Y [out] Year.
 * @param M [out] Month in [1..12].
 * @param D [out] Day in [1..31].
 */
inline void daysToYmd(int32_t days, int32_t& Y, int32_t& M, int32_t& D) {
    int32_t y = 1970;
    while (true) {
        int32_t len = 365 + (isLeap(y) ? 1 : 0);
        if (days >= len) {
            days -= len;
            ++y;
        } else
            break;
    }
    int32_t m = 1;
    while (true) {
        int32_t len = monthDays(y, m);
        if (days >= len) {
            days -= len;
            ++m;
        } else
            break;
    }
    int32_t d = days + 1;
    Y = y;
    M = m;
    D = d;
}

/**
 * @brief Add a signed offset (in days) to a "YYYY-MM-DD" date (generic arithmetic).
 *
 * @param s Start date, "YYYY-MM-DD".
 * @param off Day offset (positive or negative).
 * @return New date as "YYYY-MM-DD". If parsing fails, returns the input string.
 */
inline std::string addDays(const std::string& s, int32_t off) {
    int32_t y, m, d;
    if (!parseYmd(s, y, m, d))
        return s;
    int32_t base = ymdToDays(y, m, d);
    int32_t tgt = base + off;
    int32_t Y, M, D;
    daysToYmd(tgt, Y, M, D);
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << Y << "-" << std::setw(2) << M << "-" << std::setw(2) << D;
    return oss.str();
}

/**
 * @brief Difference in days between two dates.
 *
 * @param a Date A, "YYYY-MM-DD".
 * @param b Date B, "YYYY-MM-DD".
 * @return a - b in days (positive if a > b, negative if a < b).
 */
inline int32_t diffDays(const std::string& a, const std::string& b) {
    int32_t ya, ma, da, yb, mb, db;
    if (!parseYmd(a, ya, ma, da) || !parseYmd(b, yb, mb, db))
        return 0;
    return ymdToDays(ya, ma, da) - ymdToDays(yb, mb, db);
}

/**
 * @brief First day of the month for a given date.
 *
 * @param s Any date, "YYYY-MM-DD".
 * @return "YYYY-MM-01" (or the input if parsing fails).
 */
inline std::string startOfMonth(const std::string& s) {
    int32_t y, m, d;
    if (!parseYmd(s, y, m, d))
        return s;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << y << "-" << std::setw(2) << m << "-01";
    return oss.str();
}

/**
 * @brief Last day of the month for a given date.
 *
 * Leap years are handled for February.
 *
 * @param s Any date, "YYYY-MM-DD".
 * @return "YYYY-MM-DD" with the month's last day (or the input if parsing fails).
 */
inline std::string endOfMonth(const std::string& s) {
    int32_t y, m, d;
    if (!parseYmd(s, y, m, d))
        return s;
    int32_t md = monthDays(y, m);
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << y << "-" << std::setw(2) << m << "-" << std::setw(2) << md;
    return oss.str();
}

/**
 * @brief Extract a "YYYY-MM" year-month key from a date string.
 *
 * @param s Date "YYYY-MM-DD" (or at least "YYYY-MM").
 * @return "YYYY-MM" (or s if the string is shorter than 7 chars).
 */
inline std::string ymKey(const std::string& s) {
    return (s.size() >= 7) ? s.substr(0, 7) : s;
}

/**
 * @brief Convert a "day index in month" to a concrete date (SQL-like interpretation).
 *
 * Rules:
 *  - day >= 1 : treat it as a day-of-month (1..31) relative to the 1st
 *               => offset = (day - 1) from the first day of the month
 *  - day <  1 : treat it as a negative offset from the 1st
 *               => e.g., -2 means "two days before the 1st"
 *  - day == 0 : invalid in this 1..31 model; returns firstOfMonth as fallback
 *
 * Examples for firstOfMonth = "2025-08-01":
 *  - day =  1 -> "2025-08-01"
 *  - day =  2 -> "2025-08-02"
 *  - day = -1 -> "2025-07-31"
 *  - day = -2 -> "2025-07-30"
 *
 * @param day SQL-like day index (1..31 or negative offset).
 * @param firstOfMonth The date representing the 1st of the target month ("YYYY-MM-DD").
 * @return Concrete "YYYY-MM-DD" date according to the rules above.
 */
inline std::string dayOfMonthToDate(int32_t day, const std::string& firstOfMonth) {
    if (day == 0) {
        // Invalid in a 1..31 day-of-month model; choose your policy here.
        // For safety, return the 1st as a neutral fallback.
        return firstOfMonth;
    }
    if (day >= 1) {
        return addDays(firstOfMonth, day - 1);
    } else {
        return addDays(firstOfMonth, day);
    }
}

class Date {
private:
    int32_t m_year{};
    int32_t m_month{};
    int32_t m_day{};
    bool m_valid{};

public:
    Date() = default;
    explicit Date(const std::string& vDate) {
        m_valid = parseYmd(vDate, m_year, m_month, m_day);
    }    
    int32_t getYear() const { return m_year; }
    int32_t getMonth() const { return m_month; }
    int32_t getDay() const { return m_day; }
    Date& setYear(const int32_t vYear) {
        m_year = vYear;
        return *this;
    }
    Date& setMonth(const int32_t vMonth) {
        if (vMonth < 1) {
            m_month = 1;
        } else if (vMonth > 12) {
            m_month = 12;
        } else {
            m_month = vMonth;
        }
        return *this;
    }
    Date& setDay(const int32_t vDay) {
        const int32_t md = monthDays(m_year, m_month);  // 28/29/30/31 according to (Y,M)
        if (vDay < 1) {
            m_day = 1;
        } else if (vDay > md) {
            m_day = md;
        } else {
            m_day = vDay;
        }
        return *this;
    }
    Date& offsetYear(const int32_t vOffset) {
        m_year += vOffset;
        return *this;
    }
    Date& offsetMonth(const int32_t vOffset) {
        // 1) Mois absolu 0-based (janvier=0, ..., décembre=11)
        const int64_t totalMonths = int64_t(m_year) * 12 + (m_month - 1) + int64_t(vOffset);
        // 2) Division "plancher" (floor) pour gérer les valeurs négatives correctement
        const int64_t newYear = (totalMonths >= 0) ? (totalMonths / 12) : ((totalMonths - 11) / 12);
        // 3) Reste 0..11 puis retour en 1..12
        const int32_t newMonth = int32_t(totalMonths - newYear * 12) + 1;
        m_year = int32_t(newYear);
        m_month = newMonth;
        return *this;
    }
    Date& offsetDay(const int32_t vOffset) {
        int32_t base = ymdToDays(m_year, m_month, m_day);
        int32_t tgt = base + vOffset;
        daysToYmd(tgt, m_year, m_month, m_day);
        return *this;
    }
    std::string getDate() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(4) << m_year << "-" << std::setw(2) << m_month << "-" << std::setw(2) << m_day;
        return oss.str();
    }
};

}  // namespace date
}  // namespace ez
