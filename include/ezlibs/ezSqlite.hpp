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
#include <sstream>
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
    std::string m_table;
    struct Field {
        std::string key;
        std::string value;
        explicit Field(const std::string& vKey, const std::string& vValue) : key(vKey), value(vValue) {}
    };
    std::vector<Field> m_fields;

public:
    QueryBuilder& setTable(const std::string& vTable) {
        m_table = vTable;
        return *this;
    }
    QueryBuilder& addField(const std::string& vKey, const std::string& vValue) {
        m_fields.emplace_back(vKey, vValue);
        return *this;
    }
    template <typename T>
    QueryBuilder& addField(const std::string& vKey, const T& vValue) {
        m_fields.emplace_back(vKey, ez::str::toStr(vValue));
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
        auto query = "INSERT INTO " + m_table + " (";
        for (const auto& field : m_fields) {
            if (field.key != m_fields.at(0).key) {
                query += ", ";
            }
            query += field.key;
        }
        query += ") SELECT ";
        for (const auto& field : m_fields) {
            if (field.value != m_fields.at(0).value) {
                query += ", ";
            }
            query += "'" + field.value + "'";
        }
        query += " WHERE NOT EXISTS (SELECT 1 FROM " + m_table + " WHERE ";
        for (const auto& field : m_fields) {
            if (field.value != m_fields.at(0).value) {
                query += " and ";
            }
            query += field.key + " = '" + field.value + "'";
        }
        query += ");";
        return query;
    }
};

}  // namespace sqlite
}  // namespace ez
