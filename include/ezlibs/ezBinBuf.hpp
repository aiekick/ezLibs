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

// ezBinBuf is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace ez {
class BinBuf {
public:
    // pos is BYTES count
    
    ///////////////////////////////////////////////////
    ////// LITTLE / BIG ENDIAN INTERFACE //////////////
    ///////////////////////////////////////////////////

    template <typename T>
    size_t writeValueLE(const T& value) {
        return m_writeValue<T>(value, false);
    }

    template <typename T>
    size_t writeValueBE(const T& value) {
        return m_writeValue<T>(value, true);
    }

    template <typename T>
    size_t writeArrayLE(const T* data, size_t count) {
        return m_writeArray<T>(data, count, false);
    }

    template <typename T>
    size_t writeArrayBE(const T* data, size_t count) {
        return m_writeArray<T>(data, count, true);
    }

    template <typename T>
    T readValueLE(size_t& pos) const {
        return m_readValue<T>(pos, false);
    }

    template <typename T>
    T readValueBE(size_t& pos) const {
        return m_readValue<T>(pos, true);
    }

    template <typename T>
    void readArrayLE(size_t& pos, T* out, size_t count) const {
        m_readArray<T>(pos, out, count, false);
    }

    template <typename T>
    void readArrayBE(size_t& pos, T* out, size_t count) const {
        m_readArray<T>(pos, out, count, true);
    }

    ///////////////////////////////////////////////////
    ////// GESTION DU BUFFER //////////////////////////
    ///////////////////////////////////////////////////

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

    uint8_t& at(size_t index) {
        if (index >= m_buffer.size()) {
            throw std::out_of_range("BinBuf index out of range");
        }
        return m_buffer.at(index);
    }

private:
    std::vector<uint8_t> m_buffer;

    ///////////////////////////////////////////////////
    ////// FONCTIONS INTERNES FACTORISÉES /////////////
    ///////////////////////////////////////////////////

    inline size_t m_endianIndex(size_t i, size_t size, bool bigEndian) const { return bigEndian ? (size - 1 - i) : i; }

    template <typename T>
    size_t m_writeValue(const T& value, bool bigEndian) {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i) {
            m_buffer.push_back(ptr[m_endianIndex(i, sizeof(T), bigEndian)]);
        }
        return m_buffer.size();
    }

    template <typename T>
    size_t m_writeArray(const T* data, size_t count, bool bigEndian) {
        for (size_t i = 0; i < count; ++i) {
            m_writeValue<T>(data[i], bigEndian);
        }
        return m_buffer.size();
    }

    template <typename T>
    T m_readValue(size_t& pos, bool bigEndian) const {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        if (pos + sizeof(T) > m_buffer.size()) {
            throw std::out_of_range("Read position out of bounds");
        }

        T result = 0;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&result);
        for (size_t i = 0; i < sizeof(T); ++i) {
            ptr[i] = m_buffer[pos + m_endianIndex(i, sizeof(T), bigEndian)];
        }
        pos += sizeof(T);
        return result;
    }

    template <typename T>
    void m_readArray(size_t& pos, T* out, size_t count, bool bigEndian) const {
        for (size_t i = 0; i < count; ++i) {
            out[i] = m_readValue<T>(pos, bigEndian);
        }
    }
};


}  // namespace ez
