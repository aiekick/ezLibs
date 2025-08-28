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

#include <string>
#include <vector>
#include <unordered_map>

namespace ez {
namespace cnt {

// this class can be indexed like a vector
// and searched in like a dico
template <typename TKey, typename TValue = TKey>
class DicoVector {
protected:
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
    const TValue& operator[](const size_t& vIdx) const { return m_array[vIdx]; }
    const TValue& at(const size_t& vIdx) const { return m_array.at(vIdx); }
    std::unordered_map<TKey, size_t>& getDico() { return m_dico; }
    const std::unordered_map<TKey, size_t>& getDico() const { return m_dico; }
    std::vector<TValue>& getArray() { return m_array; }
    const std::vector<TValue>& getArray() const { return m_array; }
    typename std::vector<TValue>::iterator begin() { return m_array.begin(); }
    typename std::vector<TValue>::const_iterator begin() const { return m_array.begin(); }
    typename std::vector<TValue>::iterator end() { return m_array.end(); }
    typename std::vector<TValue>::const_iterator end() const { return m_array.end(); }
    bool exist(const TKey& vKey) const { return (m_dico.find(vKey) != m_dico.end()); }
    TValue& value(const TKey& vKey) { return at(m_dico.at(vKey)); }
    const TValue& value(const TKey& vKey) const { return at(m_dico.at(vKey)); }
    void resize(const size_t vNewSize) { m_array.resize(vNewSize); }
    void resize(const size_t vNewSize, const TValue& vVal) { m_array.resize(vNewSize, vVal); }
    void reserve(const size_t vNewCapacity) { m_array.reserve(vNewCapacity); }
    bool erase(const TKey& vKey) {
        if (exist(vKey)) {
            // when we erase an entry, there is as issue 
            // in the vector because the indexs are not more corresponding
            auto idx = m_dico.at(vKey);
            for (auto& it : m_dico) {
                // we must modify all index greater than the index to delete
                if (it.second > idx) {
                    --it.second;
                }
            }
            // now we can safely erase the item from both dico and vector
            m_array.erase(m_array.begin() + idx);
            m_dico.erase(vKey);
            return true;
        }
        return false;
    }
    bool tryAdd(const TKey& vKey, const TValue& vValue) {
        if (!exist(vKey)) {
            m_dico[vKey] = m_array.size();
            m_array.push_back(vValue);
            return true;
        }
        return false;
    }
    template <typename = std::enable_if<std::is_same<TKey, TValue>::value>>
    bool tryAdd(const TKey& vKeyValue) { return tryAdd(vKeyValue, vKeyValue); }
    bool trySetExisting(const TKey& vKey, const TValue& vValue) {
        if (exist(vKey)) {
            auto row = m_dico.at(vKey);
            m_array[row] = vValue;
            return true;
        }
        return false;
    }
    template <typename = std::enable_if<std::is_same<TKey, TValue>::value>>
    bool trySetExisting(const TKey& vKeyValue) { return trySetExisting(vKeyValue, vKeyValue); }
    // the merge can be partialy done, if already key was existing
    bool tryMerge(const DicoVector<TKey, TValue>& vDico) {
        bool ret = false;
        for (const auto& it : vDico.m_dico) {
            ret |= tryAdd(it.first, vDico.at(it.second));
        }
        return ret;
    }
};

}  // namespace cnt
}  // namespace ez
