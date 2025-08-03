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

// ezSqlite is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>//stringstream
#include <cstdarg> // variadic
#include "ezStr.hpp"

namespace ez {
namespace sqlite {

#ifdef SQLITE_API
inline std::string readStringColumn(sqlite3_stmt* vStmt, int32_t vColumn) {
    const char* str = reinterpret_cast<const char*>(sqlite3_column_text(vStmt, vColumn));
    if (str != nullptr) {
        return str;
    }
    return {};
}
#endif

// to test
// Optionnal Insert Query
// this query replace INSERT OR IGNORE who cause the auto increment
// of the AUTOINCREMENT KEY even if not inserted and ignored
/*
 * INSERT INTO banks (number, name)
 * SELECT '30003', 'LCL'
 * WHERE NOT EXISTS (
 *     SELECT 1 FROM banks WHERE number = '30002' and name = 'LCL'
 * );
 *
 * give :
 * QueryBuilder().
 *  setTable("banks").
 *  addField("number", 30002).
 *  addField("name", "LCL").
 *  build(QueryType::INSERT_IF_NOT_EXIST);
 */
// ex : build("banks", {{"number", "30002"},{"name","LCL"}});

enum class QueryType { INSERT_IF_NOT_EXIST = 0 };

class QueryBuilder {
private:
    class Field {
    private:
        std::string m_key;
        std::string m_value;
        bool m_subQuery = false;

    public:
        explicit Field(const std::string& vKey, const std::string& vValue, const bool vSubQuery) : m_key(vKey), m_value(vValue), m_subQuery(vSubQuery) {}
        const std::string& getRawKey() const { return m_key; }
        const std::string& getRawValue() const { return m_value; }
        std::string getFinalValue() const {
            if (m_subQuery) {
                return "(" + m_value + ")";
            }
            return "\"" + m_value + "\"";
        }
    };

    std::string m_table;
    std::vector<Field> m_fields;

public:
    QueryBuilder& setTable(const std::string& vTable) {
        m_table = vTable;
        return *this;
    }
    QueryBuilder& addField(const std::string& vKey, const std::string& vValue) {
        m_fields.emplace_back(vKey, vValue, false);
        return *this;
    }
    QueryBuilder& addField(const std::string& vKey, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        static char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 3072, fmt, args);
        va_end(args);
        if (w) {
            m_fields.emplace_back(vKey, std::string(TempBuffer, (size_t)w), false);
        }
        return *this;
    }
    template <typename T>
    QueryBuilder& addField(const std::string& vKey, const T& vValue) {
        m_fields.emplace_back(vKey, ez::str::toStr<T>(vValue), false);
        return *this;
    }
    QueryBuilder& addFieldQuery(const std::string& vKey, const std::string& vValue) {
        m_fields.emplace_back(vKey, vValue, true);
        return *this;
    }
    QueryBuilder& addFieldQuery(const std::string& vKey, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        static char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 3072, fmt, args);
        va_end(args);
        if (w) {
            m_fields.emplace_back(vKey, std::string(TempBuffer, (size_t)w), true);
        }
        return *this;
    }
    template <typename T>
    QueryBuilder& addFieldQuery(const std::string& vKey, const T& vValue) {
        m_fields.emplace_back(vKey, ez::str::toStr<T>(vValue), true);
        return *this;
    }
    std::string build(const QueryType vType) {
        switch (vType) {
            case QueryType::INSERT_IF_NOT_EXIST: return m_buildTypeInsertIfNotExist();
        }
        return {};
    }

private:
    std::string m_buildTypeInsertIfNotExist() {
        std::string query = "INSERT INTO " + m_table + " (";
        size_t idx = 0;
        for (const auto& field : m_fields) {
            if (idx!=0) {
                query += ", ";
            }
            query += field.getRawKey();
            ++idx;
        }
        query += ") SELECT ";
        idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += ", ";
            }
            query += field.getFinalValue();
            ++idx;
        }
        query += " WHERE NOT EXISTS (SELECT 1 FROM " + m_table + " WHERE ";
        idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += " and ";
            }
            query += field.getRawKey() + " = " + field.getFinalValue();
            ++idx;
        }
        query += ");";
        return query;
    }
};

}  // namespace sqlite
}  // namespace ez
