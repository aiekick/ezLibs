#pragma once

#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace ez {

template <typename TChar, size_t StackChunkSize = 256, size_t HeapChunkSize = StackChunkSize>
class StackString {
    static_assert(std::is_trivial<TChar>::value, "TChar must be a trivial type");

    size_t _heapSize{};  // capacity allocated in _heapData
    size_t _heapLength{};  // actual length used in _heapData
    TChar* _heapData{};  // heap buffer (null if using stack)
    size_t _stackLength{};  // actual length in _stackData
    TChar _stackData[StackChunkSize + 1]{};  // stack buffer (+1 for null terminator)

public:
    static constexpr size_t npos = static_cast<size_t>(-1);

public:
    StackString() = default;
    explicit StackString(const TChar* str) { append(str); }

    StackString(const StackString& other) { append(other.c_str()); }

    StackString(StackString&& other) noexcept : _heapSize(other._heapSize), _heapLength(other._heapLength), _heapData(other._heapData), _stackLength(other._stackLength) {
        std::memcpy(_stackData, other._stackData, sizeof(_stackData));
        other._heapData = nullptr;
        other._heapSize = 0;
        other._heapLength = 0;
        other._stackLength = 0;
        other._stackData[0] = TChar(0);
    }

    StackString& operator=(const StackString& other) {
        if (this != &other) {
            clear();
            append(other.c_str());
        }
        return *this;
    }

    StackString& operator=(StackString&& other) {
        if (this != &other) {
            clear();
            delete[] _heapData;
            _heapSize = other._heapSize;
            _heapLength = other._heapLength;
            _heapData = other._heapData;
            _stackLength = other._stackLength;
            std::memcpy(_stackData, other._stackData, sizeof(_stackData));
            other._heapData = nullptr;
            other._heapSize = 0;
            other._heapLength = 0;
            other._stackLength = 0;
            other._stackData[0] = TChar(0);
        }
        return *this;
    }

    ~StackString() { delete[] _heapData; }

    void clear() {
        _stackLength = 0;
        _stackData[0] = TChar(0);
        delete[] _heapData;
        _heapData = nullptr;
        _heapSize = 0;
        _heapLength = 0;
    }

    bool usingHeap() const { return _heapData != nullptr; }
    const TChar* c_str() const { return _heapData ? _heapData : _stackData; }
    size_t size() const { return _heapData ? _heapLength : _stackLength; }

    void reserve(size_t newSize) {
        if (HeapChunkSize == 0) {
            if (newSize > StackChunkSize) {
                throw std::runtime_error("Heap allocation disabled");
            }
            return;
        }
        if (_heapData) {
            if (newSize <= _heapSize) {
                return;
            }
            size_t newCap = ((newSize / HeapChunkSize) + 1) * HeapChunkSize;
            TChar* newData = new TChar[newCap + 1];
            std::memcpy(newData, _heapData, _heapLength * sizeof(TChar));
            newData[_heapLength] = TChar(0);
            delete[] _heapData;
            _heapData = newData;
            _heapSize = newCap;
        } else {
            if (newSize <= StackChunkSize) {
                return;
            }
            size_t newCap = ((newSize / HeapChunkSize) + 1) * HeapChunkSize;
            _heapData = new TChar[newCap + 1];
            std::memcpy(_heapData, _stackData, _stackLength * sizeof(TChar));
            _heapLength = _stackLength;
            _heapData[_heapLength] = TChar(0);
            _heapSize = newCap;
        }
    }

    void append(const TChar* str) {
        const size_t len = std::char_traits<TChar>::length(str);
        const size_t currentLen = size();
        const size_t totalLen = currentLen + len;
        reserve(totalLen);
        if (_heapData) {
            std::memcpy(_heapData + _heapLength, str, len * sizeof(TChar));
            _heapLength += len;
            _heapData[_heapLength] = TChar(0);
        } else {
            std::memcpy(_stackData + _stackLength, str, len * sizeof(TChar));
            _stackLength += len;
            _stackData[_stackLength] = TChar(0);
        }
    }

    void append(TChar c) {
        TChar tmp[2] = {c, TChar(0)};
        append(tmp);
    }

    StackString& operator+=(const TChar* str) {
        append(str);
        return *this;
    }

    StackString& operator+=(TChar c) {
        append(c);
        return *this;
    }

    TChar& operator[](size_t index) {
        if (index >= size()) {
            throw std::out_of_range("StackString index out of range");
        }
        return _heapData ? _heapData[index] : _stackData[index];
    }

    const TChar& at(size_t index) const {
        if (index >= size()) {
            throw std::out_of_range("StackString index out of range");
        }
        return _heapData ? _heapData[index] : _stackData[index];
    }

    size_t find(const TChar* needle, size_t pos = 0) const {
        const TChar* haystack = c_str();
        const size_t hayLen = size();
        const size_t needleLen = std::char_traits<TChar>::length(needle);
        if (needleLen == 0 || pos > hayLen || needleLen > hayLen - pos) {
            return npos;
        }
        for (size_t i = pos; i <= hayLen - needleLen; ++i) {
            if (std::char_traits<TChar>::compare(haystack + i, needle, needleLen) == 0) {
                return i;
            }
        }
        return npos;
    }

    StackString substr(size_t pos, size_t count = npos) const {
        const size_t len = size();
        if (pos > len) {
            throw std::out_of_range("substr position out of range");
        }
        size_t actualCount = (count == npos || pos + count > len) ? len - pos : count;
        const TChar* src = c_str() + pos;
        TChar* tmp = new TChar[actualCount + 1];
        std::memcpy(tmp, src, actualCount * sizeof(TChar));
        tmp[actualCount] = TChar(0);

        StackString result(tmp);
        delete[] tmp;
        return result;
    }
};

}  // namespace ez
