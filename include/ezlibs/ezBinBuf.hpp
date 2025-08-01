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

// ezBinBuf is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace ez {

class BinBuf {
public:
    // little-endian
    template <typename T>
    void writeLE(const T& value) {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i) {
            m_buffer.push_back(ptr[i]);
        }
    }

    // little-endian
    template <typename T>
    T readLE(size_t pos) const {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        if (pos + sizeof(T) > buffer.size()) {
            throw std::out_of_range("Read position out of bounds");
        }
        T result = 0;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&result);
        for (size_t i = 0; i < sizeof(T); ++i) {
            ptr[i] = buffer[pos + i];
        }
        return result;
    }

    // big-endian
    template <typename T>
    void writeBE(const T& value) {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i) {
            m_buffer.push_back(ptr[sizeof(T) - 1 - i]);
        }
    }

    // big-endian
    template <typename T>
    T readBE(size_t pos) const {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        if (pos + sizeof(T) > buffer.size()) {
            throw std::out_of_range("Read position out of bounds");
        }
        T result = 0;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&result);
        for (size_t i = 0; i < sizeof(T); ++i) {
            ptr[i] = buffer[pos + sizeof(T) - 1 - i];
        }
        return result;
    }

    void setDatas(const std::vector<uint8_t>& vDatas) { m_buffer = vDatas; }

    const std::vector<uint8_t>& getDatas() const { return m_buffer; }

    void clear() { m_buffer.clear(); }

    size_t size() const { return m_buffer.size(); }

    void reserve(size_t capacity) { m_buffer.reserve(capacity); }

    uint8_t operator[](size_t index) const {
        if (index >= m_buffer.size()) {
            throw std::out_of_range("BinBuf index out of range");
        }
        return m_buffer[index];
    }

private:
    std::vector<uint8_t> m_buffer;
};

}  // namespace ez
