#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

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

// ezCron is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <set>
#include <ctime>
#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iterator>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

namespace ez {
namespace time {

// Supported Crontab format
//  'mm hh dd MM DD'
//  mm : the minutes from 0 to 59
//  hh : the hour from 0 to 23
//  dd : the day of the month from 1 to 31
//  MM : the month from 1 to 12
//  DD : the day of the week from 0 to 7. 0 and 7 are the sunday
// For each fields, thoses forms are accepted :
//  * : always valid units (0,1,3,4, etc..)
//  5,8 : the units 5 and 8
//  2-5 : the units from 2 to 5 (2, 3, 4, 5)
//  */3 : all the 3 interval units(0, 3, 6, 9, etc..)

class Cron {
    friend class TestCron;

public:
    enum ErrorFlags {  //
        NONE = (0),
        INVALID_MINUTE = (1 << 0),
        INVALID_HOUR = (1 << 1),
        INVALID_MONTH_DAY = (1 << 2),
        INVALID_MONTH = (1 << 3),
        INVALID_WEEK_DAY = (1 << 4),
        INVALID_FIELDS_COUNT = (1 << 5),
        INVALID_CHAR = (1 << 6),
        INVALID_RANGE = (1 << 7),
        INVALID_INTERVAL = (1 << 8),
        INVALID_VALUES = (1 << 9),
        INVALID_FIELD = (1 << 10)
    };
    enum class FieldType {  //
        VALUE = 0,          // Value
        INTERVAL,           // Interval. each values
        RANGE,              // Range [min:max]
        VALUES,             // Values [v0,v1,v2]
        WILDCARD,           // Valid value for all
        Count
    };
    enum class FieldIndex {  //
        MINUTE = 0,
        HOUR,
        DAY_MONTH,
        MONTH,
        DAY_WEEK,
        Count
    };
    struct Field {
        std::string str;                                            // field str
        FieldIndex index{FieldIndex::MINUTE};                       // field index
        int32_t cpos{-1};                                           // char pos in the cron rule
        FieldType type{FieldType::VALUE};                           // field type
        int32_t value{-1};                                          // the value with type is 'value'
        int32_t interval{-1};                                       // the interval value when type is 'interval'
        std::pair<int32_t, int32_t> range{std::make_pair(-1, -1)};  // the range when type is 'range'
        std::set<int32_t> values{};                                 // the valude when type is 'values'
    };
    struct ErrorDetail {
        int32_t flag{};
        int32_t position{-1};
        std::string message;
    };

private:
    int32_t m_errorFlags;
    std::string m_cronRule;
    std::vector<Field> m_fields;
    std::vector<ErrorDetail> m_errorDetails;

public:
    Cron(const std::string& m_cronRule)  //
        : m_errorFlags(NONE), m_cronRule(m_cronRule) {
        m_parseExpression();
    }

    bool isOk() const {
        return (m_errorFlags == NONE);
    }

    const std::string& getCronRule() const {
        return m_cronRule;
    }

    int getErrorFlags() const {
        return m_errorFlags;
    }

    const std::vector<Field>& getFields() const {
        return m_fields;
    }

    bool isTimeToAct() const {
        time_t currentTime = std::time(nullptr);
        return isTimeToAct(currentTime);
    }

    bool isTimeToAct(time_t vCurrentTime) const {
        if (isOk()) {
#ifdef _MSC_VER
            struct tm _tm;
            localtime_s(&_tm, &vCurrentTime);
            auto* currentTm = &_tm;
#else
            auto* currentTm = std::localtime(&vCurrentTime);
#endif
            return                                                          //
                m_checkTimeForField(FieldIndex::MINUTE, currentTm->tm_min) &&  //
                m_checkTimeForField(FieldIndex::HOUR, currentTm->tm_hour) &&   //
                m_checkTimeForField(FieldIndex::DAY_MONTH, currentTm->tm_mday) &&  //
                m_checkTimeForField(FieldIndex::MONTH, currentTm->tm_mon + 1) &&   //
                m_checkTimeForField(FieldIndex::DAY_WEEK, currentTm->tm_wday);
        }
        return false;
    }

    std::string getErrorMessage() const {
        std::stringstream err;
        if (m_errorDetails.empty()) {
            std::cout << "No errors found.\n";
        } else {
            err << "Errors found in cron rule : " << std::endl;
            err << m_cronRule << std::endl;
            for (auto rev_it = m_errorDetails.rbegin(); rev_it != m_errorDetails.rend(); ++rev_it) {
                if (rev_it->position >= 0) {
                    std::string marker_line(m_cronRule.size(), ' ');
                    for (auto it = m_errorDetails.begin(); it != m_errorDetails.end(); ++it) {
                        if (it->position >= 0) {
                            marker_line[it->position] = '|';
                        }
                    }
                    marker_line[rev_it->position] = '^';
                    marker_line = marker_line.substr(0, rev_it->position + 1);
                    err << marker_line << "--<| " << m_getErrorString(rev_it->flag) << " " << rev_it->message << std::endl;
                }
            }
        }
        return err.str();
    }

    std::string getSupportedFormat() const {
        return R"(
Supported Crontab format
 mm hh dd MM DD    
 mm : the minutes from 0 to 59
 hh : the hour from 0 to 23
 dd : the day of the month from 1 to 31
 MM : the month from 1 to 12
 DD : the day of the week from 0 to 7. 0 and 7 are the sunday
For each fields, thoses forms are accepted :
 * : always valid units (0,1,3,4, etc..)
 5,8 : the units 5 and 8
 2-5 : the units from 2 to 5 (2, 3, 4, 5)
 */3 : all the 3 interval units (0, 3, 6, 9, etc..)
)";
    }

private:
    typedef int32_t CharPos;
    typedef std::pair<CharPos, std::string> Token;
    std::vector<Token> m_split(const std::string& vText, const std::string& vDelimiters) {
        std::vector<Token> arr;
        if (!vText.empty()) {
            size_t start = 0;
            std::string token;
            auto end = vText.find_first_of(vDelimiters, start);
            if (end != std::string::npos) {
                while (end != std::string::npos) {
                    token = vText.substr(start, end - start);
                    arr.push_back(std::make_pair(static_cast<CharPos>(start), token));  // empty token or not
                    start = end + 1;
                    end = vText.find_first_of(vDelimiters, start);
                }
                token = vText.substr(start);
                arr.push_back(std::make_pair(static_cast<CharPos>(start), token));  // empty token or not
            }
        }
        return arr;
    }

    int32_t m_getErrorFlagFromIndex(FieldIndex vIndex) {
        return (1 << static_cast<int32_t>(vIndex));
    }

    template <typename U, typename V, typename W>
    void m_addError(U vFlag, V vCharPos, W vIndex, const std::string& vMessage = {}) {
        auto idx = static_cast<size_t>(vIndex);
        while (m_errorDetails.size() <= idx) {
            m_errorDetails.push_back({});
        }
        auto& ed = m_errorDetails.at(idx);
        ed.flag |= static_cast<int32_t>(vFlag);
        auto cp = static_cast<int32_t>(vCharPos);
        if (ed.position < 0) {
            ed.position = cp;
        } else if (cp > ed.position) {
            ed.position = cp;
        }
        if (!vMessage.empty()) {
            ed.message += vMessage;
        }
        m_errorFlags |= ed.flag;
    }
    
    bool m_fieldHaveError(Field& vInOutField) {
        auto idx = static_cast<size_t>(vInOutField.index);
        if (m_errorDetails.size() > idx) {
            return (m_errorDetails.at(idx).position > 0);
        }
        return false;
    }
    
    bool m_parseValue(Field& vInOutField, const std::string& vValue, int32_t vCharPos) {
        try {
            auto wpos = vInOutField.str.find_first_not_of("0123456789/*-,");
            if (wpos != std::string::npos) {
                m_addError(INVALID_CHAR | m_getErrorFlagFromIndex(vInOutField.index), vInOutField.cpos + wpos, vInOutField.index);
                return false;
            }
            auto v = std::stoi(vValue);
            // check min/max
            switch (vInOutField.index) {
                case FieldIndex::MINUTE: {
                    if (v < 0 || v > 59) {
                        m_addError(ErrorFlags::INVALID_MINUTE, vCharPos, vInOutField.index);
                        return false;
                    }
                } break;
                case FieldIndex::HOUR: {
                    if (v < 0 || v > 23) {
                        m_addError(ErrorFlags::INVALID_HOUR, vCharPos, vInOutField.index);
                        return false;
                    }
                } break;
                case FieldIndex::DAY_MONTH: {
                    if (v < 1 || v > 31) {
                        m_addError(ErrorFlags::INVALID_MONTH_DAY, vCharPos, vInOutField.index);
                        return false;
                    }
                } break;
                case FieldIndex::MONTH: {
                    if (v < 1 || v > 12) {
                        m_addError(ErrorFlags::INVALID_MONTH, vCharPos, vInOutField.index);
                        return false;
                    }
                } break;
                case FieldIndex::DAY_WEEK: {
                    if (v < 0 || v > 7) {
                        m_addError(ErrorFlags::INVALID_WEEK_DAY, vCharPos, vInOutField.index);
                        return false;
                    }
                } break;
                case FieldIndex::Count:
                default: break;
            }
            // set value
            switch (vInOutField.type) {
                case FieldType::VALUE: {
                    vInOutField.value = v;
                } break;
                case FieldType::INTERVAL: {
                    vInOutField.interval = v;
                } break;
                case FieldType::RANGE: {
                    if (vInOutField.range.first == -1) {
                        vInOutField.range.first = v;
                    } else {
                        vInOutField.range.second = v;
                    }
                } break;
                case FieldType::VALUES: {
                    vInOutField.values.emplace(v);
                } break;
                case FieldType::WILDCARD:
                case FieldType::Count:
                default: break;
            }
        } catch (...) {
            m_addError(m_getErrorFlagFromIndex(vInOutField.index), vCharPos, vInOutField.index);
            return false;
        }
        return true;
    }

    bool m_parseValue(Field& vInOutField, const Token& vToken) {
        return m_parseValue(vInOutField, vToken.second, vToken.first);
    }
    
    bool m_isInterval(Field& vInOutField) {
        if (!vInOutField.str.empty()) {
            auto wpos = vInOutField.str.find("*");
            if (wpos != std::string::npos) {
                auto bad_pos = vInOutField.str.find_first_not_of("0123456789/*");
                if (bad_pos != std::string::npos) {
                    m_addError(INVALID_INTERVAL | m_getErrorFlagFromIndex(vInOutField.index),  //
                               vInOutField.cpos + bad_pos,
                               vInOutField.index);
                    return true;  // we not want to continue the field check
                }
                if (wpos < (vInOutField.str.size() - 1)) {  // interval pattern '*/'
                    ++wpos;
                    if (vInOutField.str[wpos] == '/') {
                        ++wpos;
                        vInOutField.str = vInOutField.str.substr(wpos);
                        vInOutField.type = FieldType::INTERVAL;
                        if (!m_parseValue(vInOutField, vInOutField.str, static_cast<int32_t>(wpos))) {
                            m_addError(INVALID_INTERVAL, vInOutField.cpos + wpos, vInOutField.index);
                            return false;  // wildcard, stop field checking
                        }
                    } else {
                        m_addError(INVALID_INTERVAL | m_getErrorFlagFromIndex(vInOutField.index),  //
                                   vInOutField.cpos + wpos,
                                   vInOutField.index);
                    }
                } else {  // generic pattern '*'
                    vInOutField.type = FieldType::WILDCARD;
                }
                return true;  // wildcard, stop field checking
            }
        }
        return false;  // no wildcard, continue field checking
    }
    
    bool m_isRange(Field& vInOutField) {
        if (!vInOutField.str.empty()) {
            if (vInOutField.str.front() == '-') {
                m_addError(INVALID_RANGE | m_getErrorFlagFromIndex(vInOutField.index),  //
                           vInOutField.cpos,
                           vInOutField.index);
                return true;  // range, stop field checking
            } else if (vInOutField.str.back() == '-') {
                m_addError(INVALID_RANGE | m_getErrorFlagFromIndex(vInOutField.index),  //
                           vInOutField.cpos + vInOutField.str.size() - 1,
                           vInOutField.index);
                return true;  // range, stop field checking
            }
            auto tokens = m_split(vInOutField.str, "-");
            if (!tokens.empty()) {
                vInOutField.type = FieldType::RANGE;
                if (tokens.size() != 2) {
                    auto err_pos = vInOutField.str.size();
                    for (const auto& token : tokens) {
                        if (token.second.empty()) {
                            err_pos = token.first;
                            break;
                        }
                    }
                    m_addError(INVALID_RANGE | m_getErrorFlagFromIndex(vInOutField.index),  //
                               vInOutField.cpos + err_pos,
                               vInOutField.index);
                    return true;  // range, stop field checking
                }
                for (const auto& token : tokens) {
                    if (!m_parseValue(vInOutField, token)) {
                        m_addError(INVALID_RANGE, vInOutField.cpos + token.first, vInOutField.index);
                    }
                }                
                if (!m_fieldHaveError(vInOutField)) {
                    if (vInOutField.range.first == vInOutField.range.second) {                // 5-5
                        m_addError(INVALID_RANGE | m_getErrorFlagFromIndex(vInOutField.index),  //
                                   vInOutField.cpos + tokens[1].first,
                                   vInOutField.index,
                                   " End must be different than Start.");
                    } else if (vInOutField.range.first > vInOutField.range.second) {          // 5-4
                        m_addError(INVALID_RANGE | m_getErrorFlagFromIndex(vInOutField.index),  //
                                   vInOutField.cpos + tokens[1].first,
                                   vInOutField.index,
                                   " End must be greater than Start.");
                    }
                }
                return true;  // range, stop field checking
            }
        }
        return false;  // no range, continue field checking
    }
    
    bool m_isValues(Field& vInOutField) {
        if (!vInOutField.str.empty()) {
            if (vInOutField.str.front() == ',') {
                m_addError(INVALID_VALUES | m_getErrorFlagFromIndex(vInOutField.index),  //
                           vInOutField.cpos,
                           vInOutField.index);
                return true;  // range, stop field checking
            } else if (vInOutField.str.back() == ',') {
                m_addError(INVALID_VALUES | m_getErrorFlagFromIndex(vInOutField.index),  //
                           vInOutField.cpos + vInOutField.str.size() - 1,
                           vInOutField.index);
                return true;  // range, stop field checking
            }
            auto tokens = m_split(vInOutField.str, ",");
            if (!tokens.empty()) {
                vInOutField.type = FieldType::VALUES;
                if (tokens.size() == 1) {
                    m_addError(INVALID_VALUES | m_getErrorFlagFromIndex(vInOutField.index),  //
                               vInOutField.cpos + vInOutField.str.size(),
                               vInOutField.index);
                    return true;  // we not want to continue the field check
                }
                for (const auto& token : tokens) {
                    if (!m_parseValue(vInOutField, token)) {
                        m_addError(INVALID_VALUES, vInOutField.cpos + token.first, vInOutField.index);
                    }
                }
                return true;  // values, stop field checking
            }
        }
        return false;  // no values, continue field checking
    }
    
    void m_parseField(const Token& vToken) {
        Field field;
        field.str = vToken.second;
        field.index = static_cast<FieldIndex>(m_fields.size());
        field.cpos = vToken.first;
        if (!m_isInterval(field)) {
            if (!m_isRange(field)) {
                if (!m_isValues(field)) {
                    m_parseValue(field, vToken);
                }
            }
        }
        m_fields.push_back(field);
    }
    
    void m_parseExpression() {
        m_fields.clear();
        auto tokens = m_split(m_cronRule, " ");
        auto count = static_cast<size_t>(FieldIndex::Count);
        if (tokens.size() != count) {
            m_addError(INVALID_FIELDS_COUNT, tokens.back().first, tokens.size() - 1);  // put the error on the last available field
        }
        for (const auto& token : tokens) {
            m_parseField(token);
        }
    }

    std::string m_getErrorString(int32_t vFlag) const {
        std::stringstream res;
        if (vFlag & INVALID_MINUTE) {
            res << " Invalid minute.";
        }
        if (vFlag & INVALID_HOUR) {
            res << " Invalid hour.";
        }
        if (vFlag & INVALID_MONTH_DAY) {
            res << " Invalid month day.";
        }
        if (vFlag & INVALID_MONTH) {
            res << " Invalid month.";
        }
        if (vFlag & INVALID_WEEK_DAY) {
            res << " Invalid week day.";
        }
        if (vFlag & INVALID_FIELDS_COUNT) {
            res << " Invalid fields count.";
        }
        if (vFlag & INVALID_CHAR) {
            res << " Invalid char.";
        }
        if (vFlag & INVALID_INTERVAL) {
            res << " Invalid interval.";
        }
        if (vFlag & INVALID_RANGE) {
            res << " Invalid range.";
        }
        if (vFlag & INVALID_VALUES) {
            res << " Invalid values.";
        }
        if (vFlag & INVALID_FIELD) {
            res << " Invalid field.";
        }
        return res.str();
    }

    bool m_checkTimeForField(FieldIndex vFieldIndex, int32_t vValue) const {
        const auto& field = m_fields.at(static_cast<size_t>(vFieldIndex));
        switch (field.type) {
            case FieldType::WILDCARD: {  // '*' is valid for all values
                return true;
            }
            case FieldType::VALUE: { // a == v
                return (field.value == vValue);
            }
            case FieldType::INTERVAL: { // each v
                return ((vValue % field.interval) == 0);
            }
            case FieldType::RANGE: { // a > v > b
                return (vValue >= field.range.first &&  //
                        vValue <= field.range.second);
            }
            case FieldType::VALUES: { // a == v or b == v or ..
                return (field.values.find(vValue) != field.values.end());
            }
            case FieldType::Count:
            default: break;
        }
        return false;
    }
};

}  // namespace time
}  // namespace ez

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
