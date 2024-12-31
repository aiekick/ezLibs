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

// EzBmp is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <string>
#include <vector>
#include <unordered_map>

namespace ez {
namespace cnt {

// this class can be indexed like a vector 
// and searched in like a dico
template <typename TKey, typename TValue = TKey>
class DicoVector {
private:
    std::unordered_map<TKey, size_t> m_dico;
    std::vector<TValue> m_array;

public:
    void clear() {
        m_dico.clear();
        m_array.clear();
    }
    bool empty() const { return m_array.empty(); }
    size_t size() const { return m_array.size(); }
    TValue& operator[](const size_t& vIdx) { return m_array[vIdx]; }
    TValue& at(const size_t& vIdx) { return m_array.at(vIdx); }
    typename std::vector<TValue>::iterator begin() { return m_array.begin(); }
    typename std::vector<TValue>::const_iterator begin() const { return m_array.begin(); }
    typename std::vector<TValue>::iterator end() { return m_array.end(); }
    typename std::vector<TValue>::const_iterator end() const { return m_array.end(); }
    bool exist(const TKey& vKey) const { return (m_dico.find(vKey) != m_dico.end()); }
    TValue& value(const TKey& vKey) { return at(m_dico.at(vKey)); }
    void resize(const size_t vNewSize) { m_array.resize(vNewSize); }
    void resize(const size_t vNewSize, const TValue& vVal) { m_array.resize(vNewSize, vVal); }
    void reserve(const size_t vNewCapacity) { m_array.reserve(vNewCapacity); }

    typename std::enable_if<std::is_same<TKey, TValue>::value, bool>::type 
    tryAdd(const TKey& vKey) { return tryAdd(vKey, vKey); }
    bool tryAdd(const TKey& vKey, const TValue& vValue) {
        if (!exist(vKey)) {
            m_dico[vKey] = m_array.size();
            m_array.push_back(vValue);
            return true;
        }
        return false;
    }
    bool trySetExisting(const TKey& vKey, const TValue& vValue) {
        if (exist(vKey)) {
            auto row = m_dico.at(vKey);
            m_array[row] = vValue;
            return true;
        }
        return false;
    }
};

}
}  // namespace ez