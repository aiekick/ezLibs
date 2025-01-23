#pragma once

#ifndef EZ_TOOLS_VARIANT
#define EZ_TOOLS_VARIANT
#endif  // EZ_TOOLS_VARIANT

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

// ezVariant is part od the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <set>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>

namespace ez {

template <typename T>
class variant {
private:
    std::string inputtype;  // type of input data
    std::string datatype;   // real type corresponding to the data

    std::string string_value;
    bool bool_value = false;
    int int_value = 0;
    float float_value = 0.0f;
    double double_value = 0.0;
    long long_value = 0;
    uint32_t uint32_t_value = 0;
    uint64_t uint64_t_value = 0;
#ifdef EZ_TOOLS_VEC2
    vec2<T> point_value;
#endif
#ifdef EZ_TOOLS_VEC3
    vec3<T> v3_value;
#endif
#ifdef EZ_TOOLS_VEC4
    vec4<T> rect_value;
#endif
#ifdef EZ_TOOLS_AABB
    AABB<T> aabb_value;
#endif
#ifdef EZ_TOOLS_AABBCC
    AABBCC<T> aabbcc_value;
#endif
    std::vector<float> vector_float_value;
    std::vector<double> vector_double_value;
    std::vector<std::string> vector_string_value;
    std::set<std::string> set_string_value;

public:
    variant() {
        bool_value = false;
        int_value = 0;
        float_value = 0.0f;
        double_value = 0.0;
        long_value = 0;
        uint32_t_value = 0;
    }
    variant(const int& v) {
        int_value = v;
        inputtype = "int";
        datatype = inputtype;
    }
    variant(const long& v) {
        long_value = v;
        inputtype = "long";
        datatype = inputtype;
    }
    variant(const uint64_t& v) {
        uint64_t_value = v;
        inputtype = "uint64_t";
        datatype = inputtype;
    }
    variant(const uint32_t& v) {
        uint32_t_value = v;
        inputtype = "uint32_t";
        datatype = inputtype;
    }
    variant(const float& v) {
        float_value = v;
        inputtype = "float";
        datatype = inputtype;
    }
    variant(const double& v) {
        double_value = v;
        inputtype = "double";
        datatype = inputtype;
    }
    variant(const std::string& v, const std::string& dt) {
        string_value = v;
        inputtype = "string";
        datatype = dt;
    }
    variant(const std::string& v) {
        string_value = v;
        inputtype = "string";
        datatype = inputtype;
    }
    variant(const bool v) {
        bool_value = v;
        inputtype = "bool";
        datatype = inputtype;
    }
#ifdef EZ_TOOLS_VEC2
    variant(const vec2<T>& c) {
        point_value = c;
        inputtype = "vec2";
        datatype = inputtype;
    }
#endif
#ifdef EZ_TOOLS_VEC3
    variant(const vec3<T>& c) {
        v3_value = c;
        inputtype = "vec3";
        datatype = inputtype;
    }
#endif
#ifdef EZ_TOOLS_VEC4
    variant(const vec4<T>& c) {
        rect_value = c;
        inputtype = "vec4";
        datatype = inputtype;
    }
#endif
#ifdef EZ_TOOLS_AABB
    variant(const AABB<T>& c) {
        aabb_value = c;
        inputtype = "AABB";
        datatype = inputtype;
    }
#endif
#ifdef EZ_TOOLS_AABBCC
    variant(const AABBCC<T>& c) {
        aabb_value = c;
        inputtype = "AABBCC";
        datatype = inputtype;
    }
#endif
    variant(const std::vector<double>& c) {
        vector_double_value = c;
        inputtype = "vectorDouble";
        datatype = inputtype;
    }
    variant(const std::vector<float>& c) {
        vector_float_value = c;
        inputtype = "vectorFloat";
        datatype = inputtype;
    }
    variant(const std::vector<std::string>& c) {
        vector_string_value = c;
        inputtype = "vectorString";
        datatype = inputtype;
    }
    variant(const std::set<std::string>& c) {
        set_string_value = c;
        inputtype = "setString";
        datatype = inputtype;
    }
    std::string GetInputType() const {
        return inputtype;
    }
    std::string GetDataType() const {
        return datatype;
    }
    void setCustomDataType(const std::string& vDataType) {
        datatype = vDataType;
    }
    bool operator==(variant<T> v) {
        if (inputtype == v.inputtype) {
#ifdef EZ_TOOLS_VEC4
            if (inputtype == "vec4")
                return rect_value == v.rect_value;
#endif
#ifdef EZ_TOOLS_VEC3
            if (inputtype == "vec3")
                return v3_value == v.v3_value;
#endif
#ifdef EZ_TOOLS_VEC2
            if (inputtype == "vec2")
                return point_value == v.point_value;
#endif
#ifdef EZ_TOOLS_AABB
            if (inputtype == "AABB")
                return aabb_value == v.aabb_value;
#endif
#ifdef EZ_TOOLS_AABBCC
            if (inputtype == "AABBCC")
                return aabbcc_value == v.aabbcc_value;
#endif
            if (inputtype == "bool")
                return bool_value == v.bool_value;
            if (inputtype == "float")
                return ez::isEqual(float_value, v.float_value);
            if (inputtype == "double")
                return ez::isEqual(double_value, v.double_value);
            if (inputtype == "int")
                return int_value == v.int_value;
            if (inputtype == "long")
                return long_value == v.long_value;
            if (inputtype == "uint32_t")
                return uint32_t_value == v.uint32_t_value;
            if (inputtype == "uint64_t")
                return uint64_t_value == v.uint64_t_value;
            return string_value == v.string_value;
        }
        return false;
    }

    /*uint64_t GetU64(bool *success = 0)
    {
        if (inputtype == "string")
        {
            uint64_t tmp = 0;

#ifdef _MSC_VER
            int res = sscanf_s(string_value.c_str(), "%lu64", &tmp);
#else
            int res = sscanf(string_value.c_str(), "%lu64", &tmp);
#endif
            if (success)
            {
                if (res <= 0) *success = false;
                else *success = true;
            }

            //tmp = StringToNumber<size_t>(string_value);
            return tmp;
        }
        return uint64_t_value;
    }*/

    uint32_t GetU(bool* success = nullptr) const {
        if (inputtype == "string") {
            uint32_t tmp = 0;

#ifdef _MSC_VER
            const int res = sscanf_s(string_value.c_str(), "%u", &tmp);
#else
            int res = sscanf(string_value.c_str(), "%u", &tmp);
#endif
            if (success) {
                *success = res > 0;
            }

            // tmp = StringToNumber<size_t>(string_value);
            return tmp;
        }
        return uint32_t_value;
    }

    std::string GetS(char c = ';', const char* prec = "%.6f") {
#ifdef EZ_TOOLS_VEC4
        if (inputtype == "vec4") {
            return str::toStr(rect_value.x) + c + str::toStr(rect_value.y) + c + str::toStr(rect_value.z) + c + str::toStr(rect_value.w);
        }
#endif
#ifdef EZ_TOOLS_VEC3
        if (inputtype == "vec3") {
            return str::toStr(v3_value.x) + c + str::toStr(v3_value.y) + c + str::toStr(v3_value.z);
        }
#endif
#ifdef EZ_TOOLS_VEC2
        if (inputtype == "vec2") {
            return str::toStr(point_value.x) + c + str::toStr(point_value.y);
        }
#endif
#ifdef EZ_TOOLS_AABB
        if (inputtype == "AABB") {
            return str::toStr(aabb_value.lowerBound.x) + c + str::toStr(aabb_value.lowerBound.y) + c + str::toStr(aabb_value.upperBound.x) + c +
                str::toStr(aabb_value.upperBound.y);
        }
        #endif
        if (inputtype == "bool") {
            return (bool_value ? "true" : "false");
        }
        if (inputtype == "float") {
            return str::toStr(float_value);
        }
        if (inputtype == "vectorFloat") {
            std::string str;
            for (auto f : vector_float_value) {
                if (!str.empty()) {
                    str += c;
                }
                str += str::toStr(prec, f);
            }
            return str;
        }
        if (inputtype == "vectorDouble") {
            std::string str;
            for (auto f : vector_double_value) {
                if (!str.empty()) {
                    str += c;
                }
                str += str::toStr(prec, f);
            }
            return str;
        }
        if (inputtype == "double") {
            return str::toStr(double_value);
        }
        if (inputtype == "int") {
            return str::toStr(int_value);
        }
        if (inputtype == "long") {
            return str::toStr(long_value);
        }
        if (inputtype == "uint32_t") {
            return str::toStr(uint32_t_value);
        }
        return string_value;
    }
#ifdef EZ_TOOLS_VEC2
    vec2<T> GetV2(char c = ';') {
        if (inputtype == "string") {
            return vec2<T>(string_value, c);
        } else if (inputtype == "vectorString") {
        }
        return point_value;
    }
#endif
#ifdef EZ_TOOLS_VEC3
    vec3<T> GetV3(char c = ';') {
        if (inputtype == "string") {
            return vec3<T>(string_value, c);
        } else if (inputtype == "vectorString") {
        }
        return v3_value;
    }
#endif
#ifdef EZ_TOOLS_VEC4
    vec4<T> GetV4(char c = ';') {
        if (inputtype == "string") {
            return vec4<T>(string_value, c, 4, 0);  //-V112
        } else if (inputtype == "vectorString") {
        }
        return rect_value;
    }
#endif
#ifdef EZ_TOOLS_AABB
    AABB<T> GetAABB(char c = ';') {
        if (inputtype == "string") {
            return AABB<T>(string_value, c);
        }
        return aabb_value;
    }
#endif
    std::vector<float> GetVectorFloat(char c = ';') const {
        if (inputtype == "string") {
            return str::stringToNumberVector<float>(string_value, c);
        }
        return vector_float_value;
    }
    std::vector<double> GetVectorDouble(char c = ';') const {
        if (inputtype == "string") {
            return str::stringToNumberVector<double>(string_value, c);
        }
        return vector_double_value;
    }
    std::vector<T> GetVectorType(char c = ';') {
        if (inputtype == "string") {
            return str::stringToNumberVector<T>(string_value, c);
        }
        return vector_float_value;
    }
    std::vector<std::string> GetVectorString(char c = ';') const {
        if (inputtype == "string") {
            return str::splitStringToVector(string_value, c);
        }
        return vector_string_value;
    }
    std::set<std::string> GetSetString(char c = ';') const {
        if (inputtype == "string") {
            return str::splitStringToSet(string_value, c);
        }
        return set_string_value;
    }
    float GetF(const char* vLocalToRetablish = nullptr) const {
        if (inputtype == "string") {
            std::setlocale(LC_NUMERIC, "C");
            auto res = (float)std::atof(string_value.c_str());
            if (vLocalToRetablish) {
                std::setlocale(LC_NUMERIC, vLocalToRetablish);
            }
            return res;
        }
        return float_value;
    }
    double GetD(const char* vLocalToRetablish = nullptr) const {
        if (inputtype == "string") {
            std::setlocale(LC_NUMERIC, "C");
            auto res = (double)std::atof(string_value.c_str());
            if (vLocalToRetablish) {
                std::setlocale(LC_NUMERIC, vLocalToRetablish);
            }
            return res;
        }
        return double_value;
    }
    int GetI(const char* vLocalToRetablish = nullptr) const {
        if (inputtype == "string") {
            std::setlocale(LC_NUMERIC, "C");
            auto res = std::atoi(string_value.c_str());
            if (vLocalToRetablish) {
                std::setlocale(LC_NUMERIC, vLocalToRetablish);
            }
            return res;
        }
        return int_value;
    }
    long GetL(const char* vLocalToRetablish = nullptr) const {
        if (inputtype == "string") {
            std::setlocale(LC_NUMERIC, "C");
            auto res = std::atol(string_value.c_str());
            if (vLocalToRetablish) {
                std::setlocale(LC_NUMERIC, vLocalToRetablish);
            }
            return res;
        }
        return long_value;
    }
    bool GetB() const {
        if (inputtype == "string") {
            if (string_value == "true" || string_value == "1") {
                return true;
            } else {
                return false;
            }
        } else {
            return bool_value;
        }
    }
};

typedef variant<float> fvariant;   // utile pour le type de renvoi des vec2,3,4 et AABB
typedef variant<double> dvariant;  // utile pour le type de renvoi des vec2,3,4 et AABB
typedef variant<size_t> uvariant;
typedef variant<int> ivariant;

}  // namespace ez
