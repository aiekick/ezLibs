#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <sstream>

namespace ez {
class Json {
public:
    enum Type { Null, Object, Array, String, Number, Boolean };

    // -- Constructors
    Json() : type_(Null) {}
    Json(std::nullptr_t) : type_(Null) {}
    Json(bool b) : type_(Boolean), bool_value_(b) {}
    Json(double d) : type_(Number), num_value_(d) {}
    Json(const std::string& s) : type_(String), str_value_(new std::string(s)) {}
    Json(const char* s) : type_(String), str_value_(new std::string(s)) {}
    Json(const std::map<std::string, Json>& o) : type_(Object), obj_value_(new std::map<std::string, Json>(o)) {}
    Json(const std::vector<Json>& a) : type_(Array), arr_value_(new std::vector<Json>(a)) {}
    Json(const Json& other) { copyFrom(other); }
    Json& operator=(const Json& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }
    ~Json() { clear(); }

    // -- Type queries
    Type type()      const { return type_; }
    bool isNull()    const { return type_ == Null; }
    bool isObject()  const { return type_ == Object; }
    bool isArray()   const { return type_ == Array; }
    bool isString()  const { return type_ == String; }
    bool isNumber()  const { return type_ == Number; }
    bool isBoolean() const { return type_ == Boolean; }

    // -- Accessors
    bool        asBool()   const { expect(Boolean); return bool_value_; }
    double      asNumber() const { expect(Number); return num_value_; }
    const std::string& asString() const { expect(String); return *str_value_; }
    std::map<std::string, Json>&       asObject()       { expect(Object); return *obj_value_; }
    const std::map<std::string, Json>& asObject() const { expect(Object); return *obj_value_; }
    std::vector<Json>&       asArray()       { expect(Array); return *arr_value_; }
    const std::vector<Json>& asArray() const { expect(Array); return *arr_value_; }

    // -- Object / Array indexing
    Json& operator[](const std::string& key) {
        if (!isObject()) { clear(); type_ = Object; obj_value_ = new std::map<std::string, Json>(); }
        return (*obj_value_)[key];
    }
    const Json& operator[](const std::string& key) const {
        expect(Object);
        auto it = obj_value_->find(key);
        if (it == obj_value_->end()) throw std::out_of_range("Key not found");
        return it->second;
    }
    Json& operator[](size_t idx) {
        expect(Array);
        if (idx >= arr_value_->size()) throw std::out_of_range("Index out of range");
        return (*arr_value_)[idx];
    }
    const Json& operator[](size_t idx) const {
        expect(Array);
        if (idx >= arr_value_->size()) throw std::out_of_range("Index out of range");
        return (*arr_value_)[idx];
    }

    // -- Parsing
    static Json parse(const std::string& s) {
        size_t i = 0;
        return parseValue(s, i);
    }

    // -- Serialization
    std::string serialize() const {
        switch (type_) {
        case Null:   return "null";
        case Boolean:return bool_value_ ? "true" : "false";
        case Number: {
            std::ostringstream o; o << num_value_; return o.str();
        }
        case String: return "\"" + escape(*str_value_) + "\"";
        case Array: {
            std::string r = "[";
            for (size_t i = 0; i < arr_value_->size(); ++i) {
                if (i) r += ",";
                r += (*arr_value_)[i].serialize();
            }
            r += "]";
            return r;
        }
        case Object: {
            std::string r = "{";
            bool first = true;
            for (auto& kv : *obj_value_) {
                if (!first) r += ","; first = false;
                r += "\"" + escape(kv.first) + "\":" + kv.second.serialize();
            }
            r += "}";
            return r;
        }
        }
        return ""; // never
    }

private:
    Type type_;
    bool bool_value_;
    double num_value_;
    std::string* str_value_;
    std::vector<Json>* arr_value_;
    std::map<std::string, Json>* obj_value_;

    void expect(Type t) const {
        if (type_ != t) throw std::runtime_error("Bad type access");
    }
    void clear() {
        if (type_ == String)  delete str_value_;
        if (type_ == Array)   delete arr_value_;
        if (type_ == Object)  delete obj_value_;
        type_ = Null;
    }
    void copyFrom(const Json& o) {
        type_ = o.type_;
        bool_value_ = o.bool_value_;
        num_value_ = o.num_value_;
        switch (o.type_) {
        case String: obj_value_=arr_value_=nullptr; str_value_ = new std::string(*o.str_value_); break;
        case Array:  str_value_=obj_value_=nullptr; arr_value_ = new std::vector<Json>(*o.arr_value_); break;
        case Object: str_value_=arr_value_=nullptr; obj_value_ = new std::map<std::string, Json>(*o.obj_value_); break;
        default:     str_value_=arr_value_=obj_value_=nullptr; break;
        }
    }

    // -- Lex / Parse helpers
    static void skipws(const std::string& s, size_t& i) {
        while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
    }
    static Json parseValue(const std::string& s, size_t& i) {
        skipws(s, i);
        if (i >= s.size()) throw std::runtime_error("Unexpected end of input");
        char c = s[i];
        if (c == 'n') { expectLiteral(s, i, "null"); return Json(); }
        if (c == 't') { expectLiteral(s, i, "true"); return Json(true); }
        if (c == 'f') { expectLiteral(s, i, "false"); return Json(false); }
        if (c == '"') return Json(parseString(s, i));
        if (c == '{') return parseObject(s, i);
        if (c == '[') return parseArray(s, i);
        return parseNumber(s, i);
    }

    static void expectLiteral(const std::string& s, size_t& i, const char* lit) {
        size_t len = std::strlen(lit);
        if (s.compare(i, len, lit) != 0) throw std::runtime_error("Invalid literal");
        i += len;
    }
    static std::string parseString(const std::string& s, size_t& i) {
        ++i; // skip "
        std::string res;
        while (i < s.size() && s[i] != '"') {
            if (s[i] == '\\') {
                ++i;
                if (i >= s.size()) break;
                switch (s[i]) {
                case '"': res += '"'; break;
                case '\\':res += '\\';break;
                case '/': res += '/'; break;
                case 'b': res += '\b';break;
                case 'f': res += '\f';break;
                case 'n': res += '\n';break;
                case 'r': res += '\r';break;
                case 't': res += '\t';break;
                default: res += s[i]; break;
                }
            } else {
                res += s[i];
            }
            ++i;
        }
        if (i >= s.size() || s[i] != '"') throw std::runtime_error("Unterminated string");
        ++i;
        return res;
    }
    static Json parseNumber(const std::string& s, size_t& i) {
        size_t start = i;
        if (s[i] == '-') ++i;
        while (i < s.size() && std::isdigit((unsigned char)s[i])) ++i;
        if (i < s.size() && s[i] == '.') {
            ++i;
            while (i < s.size() && std::isdigit((unsigned char)s[i])) ++i;
        }
        double d = std::stod(s.substr(start, i - start));
        return Json(d);
    }
    static Json parseArray(const std::string& s, size_t& i) {
        ++i; // skip '['
        std::vector<Json> arr;
        skipws(s, i);
        if (i < s.size() && s[i] == ']') { ++i; return Json(arr); }
        while (true) {
            arr.push_back(parseValue(s, i));
            skipws(s, i);
            if (i >= s.size()) throw std::runtime_error("Unterminated array");
            if (s[i] == ']') { ++i; break; }
            if (s[i] != ',') throw std::runtime_error("Expected ',' in array");
            ++i;
        }
        return Json(arr);
    }
    static Json parseObject(const std::string& s, size_t& i) {
        ++i; // skip '{'
        std::map<std::string, Json> obj;
        skipws(s, i);
        if (i < s.size() && s[i] == '}') { ++i; return Json(obj); }
        while (true) {
            skipws(s, i);
            if (s[i] != '"') throw std::runtime_error("Expected string for key");
            std::string key = parseString(s, i);
            skipws(s, i);
            if (i >= s.size() || s[i] != ':') throw std::runtime_error("Expected ':' after key");
            ++i;
            Json val = parseValue(s, i);
            obj.emplace(std::move(key), std::move(val));
            skipws(s, i);
            if (i >= s.size()) throw std::runtime_error("Unterminated object");
            if (s[i] == '}') { ++i; break; }
            if (s[i] != ',') throw std::runtime_error("Expected ',' in object");
            ++i;
        }
        return Json(obj);
    }
    static std::string escape(const std::string& s) {
        std::string o;
        for (char c : s) {
            switch (c) {
            case '"':  o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\b': o += "\\b";  break;
            case '\f': o += "\\f";  break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            case '\t': o += "\\t";  break;
            default:   o += c;      break;
            }
        }
        return o;
    }
};

} // namespace ez
