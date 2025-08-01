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

// ezBmp is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <vector>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <stdexcept>

namespace ez {
	
class BinBuf {
public:
    // Écrit en little-endian (par défaut)
    template<typename T>
    void writeLE(const T& value) {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i)
            buffer.push_back(ptr[i]); // little-endian
    }

    // Écrit en big-endian
    template<typename T>
    void writeBE(const T& value) {
        static_assert(std::is_arithmetic<T>::value, "Only arithmetic types supported");
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i)
            buffer.push_back(ptr[sizeof(T) - 1 - i]); // big-endian
    }

    // Accès direct au buffer
    const std::vector<uint8_t>& data() const { return buffer; }

    // Efface le contenu
    void clear() { buffer.clear(); }

    // Taille actuelle
    size_t size() const { return buffer.size(); }

    // Réserve de l’espace
    void reserve(size_t capacity) { buffer.reserve(capacity); }

    // Accès à un octet spécifique
    uint8_t operator[](size_t index) const {
        if (index >= buffer.size()) throw std::out_of_range("BinBuf index out of range");
        return buffer[index];
    }

private:
    std::vector<uint8_t> buffer;
};

} // namespace ez
	