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
#include <map>
#include <sstream>  //stringstream
#include <cstdarg>  // variadic
#include <memory>
#include <limits>

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
 *  addOrSetField("number", 30002).
 *  addOrSetField("name", "LCL").
 *  build(QueryType::INSERT_IF_NOT_EXIST);
 */
// ex : build("banks", {{"number", "30002"},{"name","LCL"}});

enum class QueryType { INSERT = 0, UPDATE, INSERT_IF_NOT_EXIST, Count };

class QueryBuilder {
private:
    class Field {
    private:
        std::string m_key;
        std::string m_value;
        bool m_subQuery = false;

    public:
        Field() = default;
        explicit Field(const std::string& vKey, const std::string& vValue, const bool vSubQuery) : m_key(vKey), m_value(vValue), m_subQuery(vSubQuery) {
            const auto zero_pos = m_value.find('\0');
            if (zero_pos != std::string::npos) {
                m_value.clear();
            }
        }
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
    std::map<std::string, Field> m_dicoFields;
    std::vector<std::string> m_fields;
    std::vector<std::string> m_where;

public:
    QueryBuilder& setTable(const std::string& vTable) {
        m_table = vTable;
        return *this;
    }
    QueryBuilder& addOrSetField(const std::string& vKey, const std::string& vValue) {
        m_addKeyIfNotExist(vKey);
        m_dicoFields[vKey] = Field(vKey, vValue, false);
        return *this;
    }
    QueryBuilder& addOrSetField(const std::string& vKey, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        static char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 1024, fmt, args);
        va_end(args);
        if (w > 0) {
            m_addKeyIfNotExist(vKey);
            m_dicoFields[vKey] = Field(vKey, std::string(TempBuffer), false);
        }
        return *this;
    }
    template <typename T>
    QueryBuilder& addOrSetField(const std::string& vKey, const T& vValue) {
        m_addKeyIfNotExist(vKey);
        m_dicoFields[vKey] = Field(vKey, ez::str::toStr<T>(vValue), false);
        return *this;
    }
    QueryBuilder& addOrSetFieldQuery(const std::string& vKey, const std::string& vValue) {
        m_addKeyIfNotExist(vKey);
        m_dicoFields[vKey] = Field(vKey, vValue, true);
        return *this;
    }
    QueryBuilder& addOrSetFieldQuery(const std::string& vKey, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        static char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 1024, fmt, args);
        va_end(args);
        if (w > 0) {
            m_addKeyIfNotExist(vKey);
            m_dicoFields[vKey] = Field(vKey, std::string(TempBuffer), true);
        }
        return *this;
    }
    template <typename T>
    QueryBuilder& addWhere(const T& vValue) {
        m_where.emplace_back(ez::str::toStr<T>(vValue));
        return *this;
    }
    QueryBuilder& addWhere(const std::string& vValue) {
        m_where.emplace_back(vValue);
        return *this;
    }
    QueryBuilder& addWhere(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        static char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 1024, fmt, args);
        va_end(args);
        if (w) {
            m_where.emplace_back(std::string(TempBuffer));
        }
        return *this;
    }
    std::string build(const QueryType vType) {
        switch (vType) {
            case QueryType::INSERT: return m_buildTypeInsert();
            case QueryType::UPDATE: return m_buildTypeUpdate();
            case QueryType::INSERT_IF_NOT_EXIST: return m_buildTypeInsertIfNotExist();
            case QueryType::Count:
            default: break;
        }
        return {};
    }

private:
    void m_addKeyIfNotExist(const std::string& vKey) {
        if (m_dicoFields.find(vKey) == m_dicoFields.end()) {
            m_fields.emplace_back(vKey);
        }
    }
    std::string m_buildTypeInsert() {
        std::string query = "INSERT INTO " + m_table + " (\n\t";
        size_t idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += ",\n\t";
            }
            query += m_dicoFields.at(field).getRawKey();
            ++idx;
        }
        query += "\n) VALUES (\n\t";
        idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += ",\n\t";
            }
            query += m_dicoFields.at(field).getFinalValue();
            ++idx;
        }
        query += "\n);";
        return query;
    }
    std::string m_buildTypeInsertIfNotExist() {
        std::string query = "INSERT INTO " + m_table + " (\n\t";
        size_t idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += ",\n\t";
            }
            query += m_dicoFields.at(field).getRawKey();
            ++idx;
        }
        query += "\n) SELECT \n\t";
        idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += ",\n\t";
            }
            query += m_dicoFields.at(field).getFinalValue();
            ++idx;
        }
        query += " WHERE NOT EXISTS (SELECT 1 FROM " + m_table + "\nWHERE\n\t";
        idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += "\n\tAND ";
            }
            query += m_dicoFields.at(field).getRawKey() + " = " + m_dicoFields.at(field).getFinalValue();
            ++idx;
        }
        query += "\n);";
        return query;
    }
    std::string m_buildTypeUpdate() {
        std::string query = "UPDATE " + m_table + " SET\n\t";
        size_t idx = 0;
        for (const auto& field : m_fields) {
            if (idx != 0) {
                query += ",\n\t";
            }
            query += m_dicoFields.at(field).getRawKey() + " = " + m_dicoFields.at(field).getFinalValue();
            ++idx;
        }
        query += "\nWHERE\n\t";
        idx = 0;
        for (const auto& where : m_where) {
            if (idx != 0) {
                query += "\tAND ";
            }
            query += "(" + where + ")\n";
            ++idx;
        }
        query += ";";
        return query;
    }
};

//---------------------------------------------
// Parser
//---------------------------------------------
class Parser {
public:
    //---------------------------------------------
    // StringRef (remplace string_view pour C++11)
    //---------------------------------------------
    struct StringRef {
        const char* data;
        size_t size;
        StringRef() : data(NULL), size(0u) {}
        StringRef(const char* vData, size_t vSize) : data(vData), size(vSize) {}
        bool empty() const { return size == 0u; }
        std::string toString() const { return size ? std::string(data, size) : std::string(); }
    };

    //---------------------------------------------
    // Positions & erreurs
    //---------------------------------------------
    struct SourcePos {
        uint32_t offset;  // octets depuis le début
        uint32_t line;  // 1-based
        uint32_t column;  // 1-based (en octets)
        SourcePos() : offset(0u), line(0u), column(0u) {}
    };

    struct Error {
        SourcePos pos;
        std::string message;  // ex: "token inattendu"
        std::string expectedHint;  // ex: "attendu: FROM | VALUES"
        Error() {}
    };

    //---------------------------------------------
    // Lexèmes
    //---------------------------------------------
    // clang-format off
    enum class TokenKind : uint16_t {
        Identifier, String, Number, Blob,
        Parameter,                 // ?, ?123, :name, @name, $name

        // Mots-clés (sous-ensemble utile)
        KwSelect, KwFrom, KwWhere, KwGroup, KwBy, KwHaving,
        KwOrder, KwLimit, KwOffset, KwWith,
        KwInsert, KwInto, KwValues, KwUpdate, KwSet, KwDelete,
        KwCreate, KwTable, KwIf, KwNot, KwExists, KwPrimary, KwKey,
        KwUnique, KwCheck, KwReferences, KwWithout, KwRowid, KwOn, KwConflict,
        KwAs,

        // Opérateurs / délimiteurs
        Plus, Minus, Star, Slash, Percent, PipePipe, Amp, Pipe, Tilde,
        Shl, Shr, Eq, EqEq, Ne, Ne2, Lt, Le, Gt, Ge, Assign,
        Comma, Dot, LParen, RParen, Semicolon,

        EndOfFile,
        Unknown
    };
    // clang-format on

    struct Token {
        TokenKind kind{TokenKind::Unknown};
        SourcePos start;
        SourcePos end;  // end.offset = 1 + dernier octet inclus (pour affichage)
        StringRef lex;  // vue sur vSql (pas de copie)
    };

    //---------------------------------------------
    // Statements
    //---------------------------------------------
    struct StatementRange {
        uint32_t beginOffset{};  // inclus
        uint32_t endOffset{};  // exclus
    };

    enum class StatementKind : uint8_t { Select, Insert, Update, Delete, CreateTable, Other };

    struct Statement {
        StatementKind kind{StatementKind::Other};
        StatementRange range;
    };

    //---------------------------------------------
    // Options & rapport
    //---------------------------------------------
    struct Options {
        bool allowNestedBlockComments = false;  // /* ... /* ... */ ... */ (si true)
        bool trackAllTokens = true;  // remplir Report.tokens
        bool caseInsensitiveKeywords = true;  // LIKE SQLite
    };

    struct Report {
        bool ok{true};
        std::vector<Error> errors;
        std::vector<Statement> statements;
        std::vector<Token> tokens;  // rempli si trackAllTokens = true
    };

private:
    // --------- private (static utils)
    static bool m_isSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
    static bool m_isDigit(char c) { return c >= '0' && c <= '9'; }
    static bool m_isHex(char c) { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); }
    static bool m_isAlpha(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_'); }
    static bool m_isAlnum(char c) { return m_isAlpha(c) || m_isDigit(c); }
    static char m_up(char c) { return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 32) : c; }
    static bool m_ieq(const StringRef& a, const char* b) {
        if (!b)
            return false;
        size_t blen = 0u;
        while (b[blen] != '\0')
            ++blen;
        if (a.size != blen)
            return false;
        for (size_t i = 0; i < a.size; ++i) {
            if (m_up(a.data[i]) != m_up(b[i]))
                return false;
        }
        return true;
    }

private:
    // --------- private (vars)
    Options m_options;
    uint32_t m_sourceSize{};
    std::vector<uint32_t> m_lineStarts;  // offset du début de chaque ligne

public:
    // --------- public (methods)
    Parser() = default;
    Parser(const Options& vOptions) : m_options(vOptions) {}

    // API principale
    bool parse(const std::string& vSql, Report& vOut) {
        m_sourceSize = static_cast<uint32_t>(vSql.size());
        m_buildLineStarts(vSql);

        vOut.ok = true;
        vOut.errors.clear();
        vOut.statements.clear();
        if (m_options.trackAllTokens)
            vOut.tokens.clear();

        // 1) Lexing
        std::vector<Token> toks;
        std::vector<Error> lexErrs;
        m_lex(vSql, toks, lexErrs);

        if (m_options.trackAllTokens) {
            vOut.tokens = toks;
        }
        if (!lexErrs.empty()) {
            for (size_t i = 0; i < lexErrs.size(); ++i)
                vOut.errors.push_back(lexErrs[i]);
        }

        // 2) Split statements
        std::vector<StatementRange> ranges;
        m_splitStatements(toks, ranges);

        // 3) Détection kind + vérifs structurelles
        for (size_t i = 0; i < ranges.size(); ++i) {
            const StatementRange& r = ranges[i];
            Statement st;
            st.range = r;
            st.kind = m_detectKind(toks, r);
            // Vérifs générales: parenthèses
            m_checkParens(toks, r, vOut);

            // Vérifs par kind
            switch (st.kind) {
                case StatementKind::CreateTable: m_checkCreateTable(toks, r, vSql, vOut); break;
                case StatementKind::Insert: m_checkInsert(toks, r, vOut); break;
                case StatementKind::Update: m_checkUpdate(toks, r, vOut); break;
                case StatementKind::Delete: m_checkDelete(toks, r, vOut); break;
                case StatementKind::Select: m_checkSelect(toks, r, vOut); break;
                default: break;
            }
            vOut.statements.push_back(st);
        }

        vOut.ok = vOut.errors.empty();
        return true;  // true = le parse a tourné ; vOut.ok dit s'il y a des erreurs
    }

    bool computeLineColumn(uint32_t vOffset, uint32_t& vOutLine, uint32_t& vOutColumn) const {
        if (m_lineStarts.empty())
            return false;
        if (vOffset > m_sourceSize)
            return false;
        // recherche binaire
        uint32_t lo = 0u;
        uint32_t hi = static_cast<uint32_t>(m_lineStarts.size());
        while (lo + 1u < hi) {
            uint32_t mid = lo + ((hi - lo) >> 1);
            if (m_lineStarts[mid] <= vOffset)
                lo = mid;
            else
                hi = mid;
        }
        vOutLine = lo + 1u;  // 1-based
        vOutColumn = vOffset - m_lineStarts[lo] + 1u;
        return true;
    }

private:
    // --------- private (methods)
    void m_buildLineStarts(const std::string& vSql) {
        m_lineStarts.clear();
        m_lineStarts.push_back(0u);
        for (uint32_t i = 0; i < static_cast<uint32_t>(vSql.size()); ++i) {
            char c = vSql[i];
            if (c == '\n') {
                m_lineStarts.push_back(i + 1u);
            } else if (c == '\r') {
                if (i + 1u < vSql.size() && vSql[i + 1u] == '\n') {
                    m_lineStarts.push_back(i + 2u);
                    ++i;
                } else {
                    m_lineStarts.push_back(i + 1u);
                }
            }
        }
    }

    void m_addError(std::vector<Error>& vErrs, uint32_t vOffset, const std::string& vMsg, const std::string& vExpected) const {
        Error e;
        e.pos.offset = vOffset;
        m_assignLineCol(vOffset, e.pos.line, e.pos.column);
        e.message = vMsg;
        e.expectedHint = vExpected;
        vErrs.push_back(e);
    }

    void m_assignLineCol(uint32_t vOffset, uint32_t& vOutLine, uint32_t& vOutCol) const {
        if (!computeLineColumn(vOffset, vOutLine, vOutCol)) {
            vOutLine = 1u;
            vOutCol = vOffset + 1u;
        }
    }

    // --- Lexing ---
    void m_lex(const std::string& vSql, std::vector<Token>& vOutToks, std::vector<Error>& vOutErrs) const {
        vOutToks.clear();
        const uint32_t n = static_cast<uint32_t>(vSql.size());
        uint32_t i = 0u;

        // petit lambda pour émettre un token
        struct Emitter {
            const std::string* src;
            const Parser* self;
            std::vector<Token>* out;
            void emit(TokenKind k, uint32_t s, uint32_t e) {
                Token t;
                t.kind = k;
                t.start.offset = s;
                t.end.offset = e;
                self->m_assignLineCol(s, t.start.line, t.start.column);
                self->m_assignLineCol(e ? (e - 1u) : 0u, t.end.line, t.end.column);
                t.lex = StringRef(src->c_str() + s, (e >= s) ? (e - s) : 0u);
                out->push_back(t);
            }
        } emit = {&vSql, this, &vOutToks};

        while (i < n) {
            char c = vSql[i];

            // espaces
            if (m_isSpace(c)) {
                ++i;
                continue;
            }

            // commentaires --
            if (c == '-') {
                if (i + 1u < n && vSql[i + 1u] == '-') {
                    i += 2u;
                    while (i < n && vSql[i] != '\n' && vSql[i] != '\r')
                        ++i;
                    continue;
                }
            }
            // commentaires /* ... */
            if (c == '/') {
                if (i + 1u < n && vSql[i + 1u] == '*') {
                    uint32_t depth = 1u;
                    i += 2u;
                    while (i < n && depth > 0u) {
                        if (vSql[i] == '/' && i + 1u < n && vSql[i + 1u] == '*') {
                            if (m_options.allowNestedBlockComments) {
                                ++depth;
                                i += 2u;
                                continue;
                            }
                        }
                        if (vSql[i] == '*' && i + 1u < n && vSql[i + 1u] == '/') {
                            --depth;
                            i += 2u;
                            continue;
                        }
                        ++i;
                    }
                    if (depth > 0u) {
                        m_addError(vOutErrs, i ? (i - 1u) : 0u, "commentaire /* ... */ non fermé", "");
                    }
                    continue;
                }
            }

            // chaînes '...'
            if (c == '\'') {
                const uint32_t s = i++;
                bool closed = false;
                while (i < n) {
                    if (vSql[i] == '\'') {
                        if (i + 1u < n && vSql[i + 1u] == '\'') {
                            i += 2u;
                            continue;
                        }  // quote doublée
                        ++i;
                        closed = true;
                        break;
                    }
                    ++i;
                }
                if (!closed) {
                    m_addError(vOutErrs, s, "chaîne non fermée", "attendu: '");
                    emit.emit(TokenKind::String, s, n);
                } else {
                    emit.emit(TokenKind::String, s, i);
                }
                continue;
            }

            // blob X'ABCD'
            if (c == 'X' || c == 'x') {
                if (i + 1u < n && vSql[i + 1u] == '\'') {
                    const uint32_t s = i;
                    i += 2u;
                    bool closed = false, badHex = false;
                    uint32_t hexCount = 0u;
                    while (i < n) {
                        if (vSql[i] == '\'') {
                            ++i;
                            closed = true;
                            break;
                        }
                        if (!m_isHex(vSql[i])) {
                            badHex = true;
                            ++i;
                            continue;
                        }
                        ++hexCount;
                        ++i;
                    }
                    emit.emit(TokenKind::Blob, s, i);
                    if (!closed) {
                        m_addError(vOutErrs, s, "blob non fermé", "attendu: '");
                    } else if ((hexCount % 2u) != 0u) {
                        m_addError(vOutErrs, s, "blob hexa de longueur impaire", "");
                    } else if (badHex) {
                        m_addError(vOutErrs, s, "caractère non-hexa dans blob", "");
                    }
                    continue;
                }
            }

            // paramètres
            if (c == '?' || c == ':' || c == '@' || c == '$') {
                const uint32_t s = i++;
                if (c == '?') {
                    while (i < n && m_isDigit(vSql[i]))
                        ++i;  // ?123
                } else {
                    while (i < n && (m_isAlnum(vSql[i]) || vSql[i] == '_'))
                        ++i;  // :name @name $name
                }
                emit.emit(TokenKind::Parameter, s, i);
                continue;
            }

            // nombres
            if (m_isDigit(c) || (c == '.' && i + 1u < n && m_isDigit(vSql[i + 1u]))) {
                const uint32_t s = i;
                bool hasDot = false;
                if (c == '.') {
                    hasDot = true;
                    ++i;
                }
                while (i < n && m_isDigit(vSql[i]))
                    ++i;
                if (i < n && vSql[i] == '.' && !hasDot) {
                    hasDot = true;
                    ++i;
                    while (i < n && m_isDigit(vSql[i]))
                        ++i;
                }
                if (i < n && (vSql[i] == 'e' || vSql[i] == 'E')) {
                    ++i;
                    if (i < n && (vSql[i] == '+' || vSql[i] == '-'))
                        ++i;
                    while (i < n && m_isDigit(vSql[i]))
                        ++i;
                }
                emit.emit(TokenKind::Number, s, i);
                continue;
            }

            // identifiants quotés "..."
            if (c == '"') {
                const uint32_t s = i++;
                bool closed = false;
                while (i < n) {
                    if (vSql[i] == '"') {
                        if (i + 1u < n && vSql[i + 1u] == '"') {
                            i += 2u;
                            continue;
                        }
                        ++i;
                        closed = true;
                        break;
                    }
                    ++i;
                }
                if (!closed) {
                    m_addError(vOutErrs, s, "identifiant \"...\" non fermé", "attendu: \"");
                }
                emit.emit(TokenKind::Identifier, s, i);
                continue;
            }
            // identifiants `...` ou [ ... ]
            if (c == '`' || c == '[') {
                const char closing = (c == '`') ? '`' : ']';
                const uint32_t s = i++;
                bool closed = false;
                while (i < n) {
                    if (vSql[i] == closing) {
                        ++i;
                        closed = true;
                        break;
                    }
                    ++i;
                }
                if (!closed) {
                    std::string msg("identifiant non fermé (attendu: ");
                    msg.push_back(closing);
                    msg.push_back(')');
                    m_addError(vOutErrs, s, msg, "");
                }
                emit.emit(TokenKind::Identifier, s, i);
                continue;
            }

            // clang-format off
            // identifiants / mots-clés non quotés
            if (m_isAlpha(c)) {
                const uint32_t s = i++;
                while (i<n && (m_isAlnum(vSql[i]) || vSql[i]=='$')) ++i;

                StringRef v(vSql.c_str() + s, (i >= s) ? (i - s) : 0u);
                TokenKind k = TokenKind::Identifier;

                if (m_ieq(v,"SELECT")) k = TokenKind::KwSelect;
                else if (m_ieq(v,"FROM")) k = TokenKind::KwFrom;
                else if (m_ieq(v,"WHERE")) k = TokenKind::KwWhere;
                else if (m_ieq(v,"GROUP")) k = TokenKind::KwGroup;
                else if (m_ieq(v,"BY")) k = TokenKind::KwBy;
                else if (m_ieq(v,"HAVING")) k = TokenKind::KwHaving;
                else if (m_ieq(v,"ORDER")) k = TokenKind::KwOrder;
                else if (m_ieq(v,"LIMIT")) k = TokenKind::KwLimit;
                else if (m_ieq(v,"OFFSET")) k = TokenKind::KwOffset;
                else if (m_ieq(v,"WITH")) k = TokenKind::KwWith;
                else if (m_ieq(v,"AS")) k = TokenKind::KwAs;
                else if (m_ieq(v,"INSERT")) k = TokenKind::KwInsert;
                else if (m_ieq(v,"INTO")) k = TokenKind::KwInto;
                else if (m_ieq(v,"VALUES")) k = TokenKind::KwValues;
                else if (m_ieq(v,"UPDATE")) k = TokenKind::KwUpdate;
                else if (m_ieq(v,"SET")) k = TokenKind::KwSet;
                else if (m_ieq(v,"DELETE")) k = TokenKind::KwDelete;
                else if (m_ieq(v,"CREATE")) k = TokenKind::KwCreate;
                else if (m_ieq(v,"TABLE")) k = TokenKind::KwTable;
                else if (m_ieq(v,"IF")) k = TokenKind::KwIf;
                else if (m_ieq(v,"NOT")) k = TokenKind::KwNot;
                else if (m_ieq(v,"EXISTS")) k = TokenKind::KwExists;
                else if (m_ieq(v,"PRIMARY")) k = TokenKind::KwPrimary;
                else if (m_ieq(v,"KEY")) k = TokenKind::KwKey;
                else if (m_ieq(v,"UNIQUE")) k = TokenKind::KwUnique;
                else if (m_ieq(v,"CHECK")) k = TokenKind::KwCheck;
                else if (m_ieq(v,"REFERENCES")) k = TokenKind::KwReferences;
                else if (m_ieq(v,"WITHOUT")) k = TokenKind::KwWithout;
                else if (m_ieq(v,"ROWID")) k = TokenKind::KwRowid;
                else if (m_ieq(v,"ON")) k = TokenKind::KwOn;
                else if (m_ieq(v,"CONFLICT")) k = TokenKind::KwConflict;

                emit.emit(k, s, i);
                continue;
            }

            // opérateurs / ponctuation
            if (i+1u<n) {
                char c2 = vSql[i+1u];
                if (c=='|' && c2=='|') { emit.emit(TokenKind::PipePipe, i, i+2u); i+=2u; continue; }
                if (c=='<' && c2=='<') { emit.emit(TokenKind::Shl, i, i+2u); i+=2u; continue; }
                if (c=='>' && c2=='>') { emit.emit(TokenKind::Shr, i, i+2u); i+=2u; continue; }
                if (c=='=' && c2=='=') { emit.emit(TokenKind::EqEq, i, i+2u); i+=2u; continue; }
                if (c=='!' && c2=='=') { emit.emit(TokenKind::Ne, i, i+2u); i+=2u; continue; }
                if (c=='<' && c2=='>') { emit.emit(TokenKind::Ne2, i, i+2u); i+=2u; continue; }
                if (c=='<' && c2=='=') { emit.emit(TokenKind::Le, i, i+2u); i+=2u; continue; }
                if (c=='>' && c2=='=') { emit.emit(TokenKind::Ge, i, i+2u); i+=2u; continue; }
            }
            switch (c) {
                case '+': emit.emit(TokenKind::Plus, i, i+1u); ++i; continue;
                case '-': emit.emit(TokenKind::Minus, i, i+1u); ++i; continue;
                case '*': emit.emit(TokenKind::Star, i, i+1u); ++i; continue;
                case '/': emit.emit(TokenKind::Slash, i, i+1u); ++i; continue;
                case '%': emit.emit(TokenKind::Percent, i, i+1u); ++i; continue;
                case '&': emit.emit(TokenKind::Amp, i, i+1u); ++i; continue;
                case '|': emit.emit(TokenKind::Pipe, i, i+1u); ++i; continue;
                case '~': emit.emit(TokenKind::Tilde, i, i+1u); ++i; continue;
                case '=': emit.emit(TokenKind::Assign, i, i+1u); ++i; continue;
                case '<': emit.emit(TokenKind::Lt, i, i+1u); ++i; continue;
                case '>': emit.emit(TokenKind::Gt, i, i+1u); ++i; continue;
                case ',': emit.emit(TokenKind::Comma, i, i+1u); ++i; continue;
                case '.': emit.emit(TokenKind::Dot, i, i+1u); ++i; continue;
                case '(': emit.emit(TokenKind::LParen, i, i+1u); ++i; continue;
                case ')': emit.emit(TokenKind::RParen, i, i+1u); ++i; continue;
                case ';': emit.emit(TokenKind::Semicolon, i, i+1u); ++i; continue;
                default: break;
            }
            // clang-format on

            // inconnu
            m_addError(vOutErrs, i, "caractère inconnu", "");
            emit.emit(TokenKind::Unknown, i, i + 1u);
            ++i;
        }

        // EOF
        Token eof;
        eof.kind = TokenKind::EndOfFile;
        eof.start.offset = n;
        eof.end.offset = n;
        m_assignLineCol(n, eof.start.line, eof.start.column);
        eof.end = eof.start;
        eof.lex = StringRef(NULL, 0u);
        vOutToks.push_back(eof);
    }

    void m_splitStatements(const std::vector<Token>& vToks, std::vector<StatementRange>& vOut) const {
        vOut.clear();
        uint32_t curStart = 0u;
        uint32_t lastNonSpace = 0u;
        bool hasContent = false;

        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.kind == TokenKind::EndOfFile) {
                if (hasContent) {
                    StatementRange r;
                    r.beginOffset = curStart;
                    r.endOffset = lastNonSpace + 1u;
                    if (r.endOffset <= r.beginOffset)
                        r.endOffset = r.beginOffset;
                    vOut.push_back(r);
                }
                break;
            }
            // pas d'espaces ici (déjà consommés au lexing)
            if (!hasContent) {
                hasContent = true;
                curStart = t.start.offset;
            }
            lastNonSpace = (t.end.offset > 0u) ? (t.end.offset - 1u) : t.end.offset;

            if (t.kind == TokenKind::Semicolon) {
                if (hasContent) {
                    StatementRange r;
                    r.beginOffset = curStart;
                    r.endOffset = t.start.offset;  // avant le ';'
                    if (r.endOffset < r.beginOffset)
                        r.endOffset = r.beginOffset;
                    vOut.push_back(r);
                }
                hasContent = false;
            }
        }
    }

    StatementKind m_detectKind(const std::vector<Token>& vToks, const StatementRange& vRng) const {
        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;
            switch (t.kind) {
                case TokenKind::KwSelect: return StatementKind::Select;
                case TokenKind::KwInsert: return StatementKind::Insert;
                case TokenKind::KwUpdate: return StatementKind::Update;
                case TokenKind::KwDelete: return StatementKind::Delete;
                case TokenKind::KwCreate: return StatementKind::CreateTable;  // affiné dans check
                default: return StatementKind::Other;
            }
        }
        return StatementKind::Other;
    }

    // --- vérifs générales
    void m_checkParens(const std::vector<Token>& vToks, const StatementRange& vRng, Report& vOut) const {
        int32_t depth = 0;
        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;
            if (t.kind == TokenKind::LParen) {
                ++depth;
            } else if (t.kind == TokenKind::RParen) {
                --depth;
                if (depth < 0) {
                    m_addError(vOut.errors, t.start.offset, "parenthèse fermante sans ouvrante", "supprimer ')'");
                    depth = 0;
                }
            }
        }
        if (depth > 0) {
            m_addError(vOut.errors, vRng.endOffset, "parenthèse fermante manquante", "attendu: ')'");
        }
    }

    // --- vérifs par kind
    void m_checkCreateTable(const std::vector<Token>& vToks, const StatementRange& vRng, const std::string& /*vSql*/, Report& vOut) const {
        bool sawCreate = false, sawTable = false;
        const Token* nameTok = NULL;
        const Token* afterName = NULL;

        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;

            if (!sawCreate) {
                if (t.kind == TokenKind::KwCreate) {
                    sawCreate = true;
                } else {
                    return;
                }
                continue;
            }
            if (!sawTable) {
                if (t.kind == TokenKind::KwTable) {
                    sawTable = true;
                }
                continue;
            }
            if (!nameTok) {
                if (t.kind == TokenKind::Identifier) {
                    nameTok = &t;
                    continue;
                }
                if (t.kind == TokenKind::KwIf)
                    continue;
                if (t.kind == TokenKind::KwNot)
                    continue;
                if (t.kind == TokenKind::KwExists)
                    continue;
                m_addError(vOut.errors, t.start.offset, "nom de table attendu après CREATE TABLE", "identifiant");
                return;
            } else {
                afterName = &t;
                break;
            }
        }

        if (!nameTok) {
            m_addError(vOut.errors, vRng.beginOffset, "nom de table manquant", "identifiant");
            return;
        }
        if (!afterName) {
            m_addError(vOut.errors, nameTok->end.offset, "attendu '(' ou AS après le nom de table", "(' | AS");
            return;
        }

        bool hasParen = false, hasAs = false;
        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < afterName->start.offset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;
            if (t.kind == TokenKind::LParen) {
                hasParen = true;
                break;
            }
            if (t.kind == TokenKind::KwAs) {
                hasAs = true;
                break;
            }
        }
        if (!hasParen && !hasAs) {
            m_addError(vOut.errors, afterName->start.offset, "attendu '(' ou AS après le nom de table", "(' | AS");
        }
    }

    void m_checkInsert(const std::vector<Token>& vToks, const StatementRange& vRng, Report& vOut) const {
        bool sawInsert = false, sawInto = false, sawValues = false, sawSelect = false;
        const Token* afterValues = NULL;

        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;

            if (!sawInsert) {
                if (t.kind == TokenKind::KwInsert) {
                    sawInsert = true;
                } else {
                    return;
                }
                continue;
            }
            if (!sawInto) {
                if (t.kind == TokenKind::KwInto) {
                    sawInto = true;
                    continue;
                }
                continue;
            }
            if (!sawValues && !sawSelect) {
                if (t.kind == TokenKind::KwValues) {
                    sawValues = true;
                    continue;
                }
                if (t.kind == TokenKind::KwSelect) {
                    sawSelect = true;
                    continue;
                }
                continue;
            }
            if (sawValues && !afterValues) {
                afterValues = &t;
                break;
            }
        }

        if (!sawInsert)
            return;
        if (!sawInto) {
            m_addError(vOut.errors, vRng.beginOffset, "mot-clé INTO manquant dans INSERT", "INTO");
            return;
        }
        if (!sawValues && !sawSelect) {
            m_addError(vOut.errors, vRng.beginOffset, "INSERT incomplet", "VALUES | SELECT");
            return;
        }
        if (sawValues && afterValues) {
            int32_t depth = 0;
            bool hasPar = false;
            for (size_t i = 0; i < vToks.size(); ++i) {
                const Token& t = vToks[i];
                if (t.start.offset < afterValues->start.offset)
                    continue;
                if (t.start.offset >= vRng.endOffset)
                    break;
                if (t.kind == TokenKind::LParen) {
                    ++depth;
                    hasPar = true;
                } else if (t.kind == TokenKind::RParen) {
                    --depth;
                    if (depth < 0)
                        depth = 0;
                }
            }
            if (!hasPar) {
                m_addError(vOut.errors, afterValues->start.offset, "VALUES sans liste parenthésée", "('...')");
            }
        }
    }

    void m_checkUpdate(const std::vector<Token>& vToks, const StatementRange& vRng, Report& vOut) const {
        bool sawUpdate = false, sawSet = false;
        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;
            if (!sawUpdate) {
                if (t.kind == TokenKind::KwUpdate) {
                    sawUpdate = true;
                } else {
                    return;
                }
                continue;
            }
            if (!sawSet) {
                if (t.kind == TokenKind::KwSet) {
                    sawSet = true;
                    break;
                }
            }
        }
        if (!sawUpdate)
            return;
        if (!sawSet) {
            m_addError(vOut.errors, vRng.beginOffset, "UPDATE sans SET", "SET");
        }
    }

    void m_checkDelete(const std::vector<Token>& vToks, const StatementRange& vRng, Report& vOut) const {
        bool sawDelete = false, sawFrom = false;
        for (size_t i = 0; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset)
                continue;
            if (t.start.offset >= vRng.endOffset)
                break;
            if (!sawDelete) {
                if (t.kind == TokenKind::KwDelete) {
                    sawDelete = true;
                } else {
                    return;
                }
                continue;
            }
            if (!sawFrom) {
                if (t.kind == TokenKind::KwFrom) {
                    sawFrom = true;
                    break;
                }
            }
        }
        if (!sawDelete)
            return;
        if (!sawFrom) {
            m_addError(vOut.errors, vRng.beginOffset, "DELETE sans FROM", "FROM");
        }
    }

    void m_checkSelect(const std::vector<Token>& vToks, const StatementRange& vRng, Report& vOut) const {
        // 1) Trouver SELECT
        size_t selIdx = static_cast<size_t>(-1);
        for (size_t i = 0u; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset < vRng.beginOffset) {
                continue;
            }
            if (t.start.offset >= vRng.endOffset) {
                break;
            }
            if (t.kind == TokenKind::KwSelect) {
                selIdx = i;
                break;
            }
        }
        if (selIdx == static_cast<size_t>(-1)) {
            return;  // pas un SELECT (sécurité)
        }

        // 2) Scanner la projection: SELECT <expr> [, <expr> ...] [FROM ... | ...]
        bool seenAny = false;
        bool expectingExpr = true;  // vrai au début / après chaque virgule
        size_t i = selIdx + 1u;

        for (; i < vToks.size(); ++i) {
            const Token& t = vToks[i];
            if (t.start.offset >= vRng.endOffset) {
                break;
            }

            const TokenKind k = t.kind;

            // Fin de projection : mots-clés majeurs ou fin
            const bool endOfProjection = (k == TokenKind::KwFrom) || (k == TokenKind::KwWhere) || (k == TokenKind::KwGroup) || (k == TokenKind::KwOrder) ||
                (k == TokenKind::KwLimit) || (k == TokenKind::KwOffset) || (k == TokenKind::Semicolon) || (k == TokenKind::EndOfFile);

            if (endOfProjection) {
                if (!seenAny) {
                    // Rien après SELECT
                    m_addError(vOut.errors, vToks[selIdx].start.offset, "projection SELECT manquante", "*, identifiant, expression");
                } else if (expectingExpr) {
                    // Virgule traînante: ex. "SELECT id, FROM ..."
                    m_addError(vOut.errors, t.start.offset, "expression de projection manquante avant ce token", "expression après ','");
                }
                break;
            }

            if (k == TokenKind::Comma) {
                if (expectingExpr) {
                    // Cas ",," ou ", FROM" (doublement signalé ici)
                    m_addError(vOut.errors, t.start.offset, "expression de projection manquante après ','", "expression");
                }
                expectingExpr = true;
                continue;
            }

            // Têtes possibles d'expression (simplifiées)
            const bool isExprHead = (k == TokenKind::Star) || (k == TokenKind::Identifier) || (k == TokenKind::Number) || (k == TokenKind::String) ||
                (k == TokenKind::Parameter) || (k == TokenKind::LParen);

            if (isExprHead) {
                seenAny = true;
                expectingExpr = false;
                continue;
            }

            // Token inattendu en position d'expression
            if (expectingExpr && !seenAny) {
                m_addError(vOut.errors, t.start.offset, "token inattendu dans la projection SELECT", "*, identifiant, expression");
                // On continue pour tenter de repérer FROM/ORDER/LIMIT
                expectingExpr = false;  // évite cascade sur ce jeton
                continue;
            }
        }

        // Si on a fini la boucle sans rencontrer fin de projection
        if (i >= vToks.size() || vToks[i].start.offset >= vRng.endOffset) {
            if (!seenAny) {
                m_addError(vOut.errors, vToks[selIdx].start.offset, "projection SELECT manquante", "*, identifiant, expression");
            } else if (expectingExpr) {
                m_addError(vOut.errors, vRng.endOffset, "expression de projection manquante en fin de SELECT", "expression après ','");
            }
        }

        // 3) Vérifier FROM (facultatif en SQLite, donc on ne l'exige pas, on valide seulement sa forme)
        for (size_t idx = selIdx + 1u; idx < vToks.size(); ++idx) {
            const Token& t = vToks[idx];
            if (t.start.offset < vRng.beginOffset) {
                continue;
            }
            if (t.start.offset >= vRng.endOffset) {
                break;
            }

            if (t.kind == TokenKind::KwFrom) {
                size_t j = idx + 1u;
                if (j >= vToks.size() || vToks[j].start.offset >= vRng.endOffset) {
                    m_addError(vOut.errors, t.start.offset, "table attendue après FROM", "identifiant ou sous-requête");
                } else {
                    const TokenKind nk = vToks[j].kind;
                    if (!(nk == TokenKind::Identifier || nk == TokenKind::LParen)) {
                        m_addError(vOut.errors, vToks[j].start.offset, "élément invalide après FROM", "identifiant ou sous-requête");
                    }
                }
                break;  // un seul FROM principal ici (checker léger)
            }
        }

        // 4) ORDER BY et LIMIT/OFFSET (inchangé)
        for (size_t idx = selIdx + 1u; idx < vToks.size(); ++idx) {
            const Token& t = vToks[idx];
            if (t.start.offset < vRng.beginOffset) {
                continue;
            }
            if (t.start.offset >= vRng.endOffset) {
                break;
            }

            if (t.kind == TokenKind::KwOrder) {
                size_t j = idx + 1u;
                bool hasBy = false;
                while (j < vToks.size() && vToks[j].start.offset < vRng.endOffset) {
                    if (vToks[j].kind == TokenKind::KwBy) {
                        hasBy = true;
                        ++j;
                        break;
                    }
                    if (vToks[j].kind != TokenKind::Comma) {
                        break;
                    }
                    ++j;
                }
                if (!hasBy) {
                    m_addError(vOut.errors, t.start.offset, "ORDER sans BY", "BY");
                } else {
                    if (j >= vToks.size() || vToks[j].start.offset >= vRng.endOffset) {
                        m_addError(vOut.errors, t.start.offset, "ORDER BY incomplet", "");
                    }
                }
            }

            if (t.kind == TokenKind::KwLimit || t.kind == TokenKind::KwOffset) {
                size_t j = idx + 1u;
                while (j < vToks.size() && vToks[j].start.offset < vRng.endOffset) {
                    const TokenKind k2 = vToks[j].kind;
                    if (k2 == TokenKind::Number || k2 == TokenKind::Parameter) {
                        break;
                    }
                    if (k2 == TokenKind::Comma) {
                        ++j;
                        continue;  // LIMIT x, y accepté
                    }
                    m_addError(vOut.errors, vToks[j].start.offset, "valeur invalide pour LIMIT/OFFSET", "nombre ou paramètre");
                    break;
                }
                if (j >= vToks.size() || vToks[j].start.offset >= vRng.endOffset) {
                    m_addError(vOut.errors, t.start.offset, "LIMIT/OFFSET sans valeur", "nombre ou paramètre");
                }
            }
        }
    }
};

}  // namespace sqlite
}  // namespace ez
