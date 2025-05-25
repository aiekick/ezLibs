#pragma once

#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace ez {
namespace json {

class Value {
private:
    enum Type { Null, Object, Array, String, Number, Boolean };

    Type m_type;
    bool m_bool;
    double m_num;
    std::string m_str;
    std::vector<Value> m_arr;
    std::map<std::string, Value> m_obj;

public:
    // -- Constructeurs
    Value() : m_type(Null), m_bool(false), m_num(0) {}
    Value(std::nullptr_t) : Value() {}
    Value(bool b) : m_type(Boolean), m_bool(b), m_num(0) {}
    Value(double d) : m_type(Number), m_bool(false), m_num(d) {}
    Value(const std::string& s) : m_type(String), m_bool(false), m_num(0), m_str(s) {}
    Value(const char* s) : Value(std::string(s)) {}
    Value(const std::map<std::string, Value>& o) : m_type(Object), m_bool(false), m_num(0), m_obj(o) {}
    Value(const std::vector<Value>& a) : m_type(Array), m_bool(false), m_num(0), m_arr(a) {}

    // -- Type-checkers
    Type type() const { return m_type; }
    bool isNull() const { return m_type == Null; }
    bool isObject() const { return m_type == Object; }
    bool isArray() const { return m_type == Array; }
    bool isString() const { return m_type == String; }
    bool isNumber() const { return m_type == Number; }
    bool isBool() const { return m_type == Boolean; }

    // -- Accès
    bool asBool() const {
        m_check(Boolean);
        return m_bool;
    }
    double asNumber() const {
        m_check(Number);
        return m_num;
    }
    const std::string& asString() const {
        m_check(String);
        return m_str;
    }
    const std::map<std::string, Value>& asObject() const {
        m_check(Object);
        return m_obj;
    }
    const std::vector<Value>& asArray() const {
        m_check(Array);
        return m_arr;
    }

    // -- Modifieurs
    std::map<std::string, Value>& asObject() {
        m_check(Object);
        return m_obj;
    }
    std::vector<Value>& asArray() {
        m_check(Array);
        return m_arr;
    }
    Value& operator[](const std::string& k) {
        if (m_type != Object) {
            m_type = Object;
            m_obj.clear();
        }
        return m_obj[k];
    }
    Value& operator[](size_t i) {
        if (m_type != Array)
            throw std::runtime_error("Not an array");
        if (i >= m_arr.size())
            throw std::out_of_range("Index out of range");
        return m_arr[i];
    }

    static Value parse(const std::string& vJson) {
        size_t i = 0;
        return m_parseValue(vJson, i);
    }

    std::string dump(bool vPretty = false, int vIndent = 4) const {
        if (!vPretty) {
            return m_dumpMinimal();
        }
        return m_dumpPretty(0, vIndent);
    }

private:
    std::string m_dumpMinimal() const {
        switch (m_type) {
            case Null: return "null";
            case Boolean: return m_bool ? "true" : "false";
            case Number: {
                std::ostringstream o;
                o << m_num;
                return o.str();
            }
            case String: return "\"" + m_escape(m_str) + "\"";
            case Array: {
                std::string r = "[";
                for (size_t i = 0; i < m_arr.size(); ++i) {
                    if (i)
                        r += ",";
                    r += m_arr[i].m_dumpMinimal();
                }
                return r + "]";
            }
            case Object: {
                std::string r = "{";
                bool first = true;
                for (auto& kv : m_obj) {
                    if (!first)
                        r += ",";
                    first = false;
                    r += "\"" + m_escape(kv.first) + "\":" + kv.second.m_dumpMinimal();
                }
                return r + "}";
            }
        }
        return "";
    }

    std::string m_dumpPretty(int level, int indent) const {
        std::string pad(level * indent, ' ');
        switch (m_type) {
            case Null: return pad + "null";
            case Boolean: return pad + (m_bool ? "true" : "false");
            case Number: {
                std::ostringstream o;
                o << pad << m_num;
                return o.str();
            }
            case String: return pad + "\"" + m_escape(m_str) + "\"";
            case Array: {
                if (m_arr.empty())
                    return pad + "[]";
                std::string r = pad + "[\n";
                for (size_t i = 0; i < m_arr.size(); ++i) {
                    r += m_arr[i].m_dumpPretty(level + 1, indent);
                    if (i + 1 < m_arr.size())
                        r += ",\n";
                }
                r += "\n" + pad + "]";
                return r;
            }
            case Object: {
                if (m_obj.empty())
                    return pad + "{}";
                std::string r = pad + "{\n";
                bool first = true;
                for (auto& kv : m_obj) {
                    if (!first)
                        r += ",\n";
                    first = false;
                    r += std::string((level + 1) * indent, ' ') + "\"" + m_escape(kv.first) + "\": " + kv.second.m_dumpPretty(level + 1, indent);
                }
                r += "\n" + pad + "}";
                return r;
            }
        }
        return pad + "";
    }

    void m_check(Type t) const {
        if (m_type != t)
            throw std::runtime_error("Bad JSON type access");
    }
    static void m_skip(const std::string& s, size_t& i) {
        while (i < s.size() && std::isspace((unsigned char)s[i]))
            ++i;
    }
    static Value m_parseValue(const std::string& s, size_t& i) {
        m_skip(s, i);
        if (i >= s.size())
            throw std::runtime_error("Unexpected end");
        switch (s[i]) {
            case 'n': m_expect(s, i, "null"); return Value(nullptr);
            case 't': m_expect(s, i, "true"); return Value(true);
            case 'f': m_expect(s, i, "false"); return Value(false);
            case '"': return Value(m_parseString(s, i));
            case '[': return m_parseArray(s, i);
            case '{': return m_parseObject(s, i);
            default: return m_parseNumber(s, i);
        }
    }
    static void m_expect(const std::string& s, size_t& i, const char* lit) {
        size_t L = std::strlen(lit);
        if (s.compare(i, L, lit) != 0)
            throw std::runtime_error("Invalid literal");
        i += L;
    }
    static std::string m_parseString(const std::string& s, size_t& i) {
        ++i;
        std::string out;
        while (i < s.size() && s[i] != '"') {
            if (s[i] == '\\') {
                ++i;
                if (i >= s.size())
                    break;
                switch (s[i]) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    default: out += s[i]; break;
                }
            } else {
                out += s[i];
            }
            ++i;
        }
        if (i >= s.size() || s[i] != '"')
            throw std::runtime_error("Unterminated string");
        ++i;
        return out;
    }
    static Value m_parseNumber(const std::string& s, size_t& i) {
        size_t start = i;
        if (s[i] == '-')
            ++i;
        while (i < s.size() && std::isdigit((unsigned char)s[i]))
            ++i;
        if (i < s.size() && s[i] == '.') {
            ++i;
            while (i < s.size() && std::isdigit((unsigned char)s[i]))
                ++i;
        }
        double d = std::stod(s.substr(start, i - start));
        return Value(d);
    }
    static Value m_parseArray(const std::string& s, size_t& i) {
        ++i;
        m_skip(s, i);
        std::vector<Value> a;
        if (i < s.size() && s[i] == ']') {
            ++i;
            return Value(a);
        }
        while (true) {
            a.push_back(m_parseValue(s, i));
            m_skip(s, i);
            if (i >= s.size())
                throw std::runtime_error("Unterminated array");
            if (s[i] == ']') {
                ++i;
                break;
            }
            if (s[i] != ',')
                throw std::runtime_error("Expected ','");
            ++i;
        }
        return Value(a);
    }
    static Value m_parseObject(const std::string& s, size_t& i) {
        ++i;
        m_skip(s, i);
        std::map<std::string, Value> o;
        if (i < s.size() && s[i] == '}') {
            ++i;
            return Value(o);
        }
        while (true) {
            m_skip(s, i);
            if (s[i] != '"')
                throw std::runtime_error("Expected key string");
            std::string key = m_parseString(s, i);
            m_skip(s, i);
            if (i >= s.size() || s[i] != ':')
                throw std::runtime_error("Expected ':'");
            ++i;
            Value v = m_parseValue(s, i);
            o.emplace(std::move(key), std::move(v));
            m_skip(s, i);
            if (i >= s.size())
                throw std::runtime_error("Unterminated object");
            if (s[i] == '}') {
                ++i;
                break;
            }
            if (s[i] != ',')
                throw std::runtime_error("Expected ','");
            ++i;
        }
        return Value(o);
    }
    static std::string m_escape(const std::string& s) {
        std::string r;
        for (char c : s) {
            switch (c) {
                case '"': r += "\\\""; break;
                case '\\': r += "\\\\"; break;
                case '\b': r += "\\b"; break;
                case '\f': r += "\\f"; break;
                case '\n': r += "\\n"; break;
                case '\r': r += "\\r"; break;
                case '\t': r += "\\t"; break;
                default: r += c; break;
            }
        }
        return r;
    }
};

}  // namespace json

class Json {
private:
    json::Value m_root;

public:
    bool parse(const std::string& txt) {
        try {
            m_root = json::Value::parse(txt);
            return true;
        } catch (...) {
            return false;
        }
    }

    json::Value& getRootRef() { return m_root; }
    const json::Value& getRoot() const { return m_root; }

    std::string dump(bool vPretty = false, int vIndent = 4) const { return m_root.dump(vPretty, vIndent); }
};

}  // namespace ez
