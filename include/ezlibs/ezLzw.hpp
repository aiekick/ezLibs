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

// EzLzw is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace ez {
namespace comp {

// LZW algo :
// https://fr.wikipedia.org/wiki/Lempel-Ziv-Welch

class Lzw {
    friend class TestLzw;

private:
    typedef uint16_t DataType;
    std::vector<DataType> m_datas;
    std::unordered_map<std::string, uint16_t> m_dicoComp;
    std::unordered_map<uint16_t, std::string> m_dicoDeComp;

public:
    Lzw() {
        std::string c;
        for (uint16_t i = 0; i < 256; ++i) {
            c = static_cast<char>(i);
            m_dicoComp[c] = i;
            m_dicoDeComp[i] = c;
        }
    }
    ~Lzw() = default;

    Lzw& compressFile(const std::string& vFilePathName) {
        std::ifstream inputFile(vFilePathName, std::ios::binary);
        if (inputFile.is_open()) {
            std::vector<DataType> buffer((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
            inputFile.close();
            m_datas = m_compress(buffer);
        }
        return *this;
    }

    Lzw& compresss(void* vBuffer, size_t vBufferLen) {
        std::vector<DataType> buffer(static_cast<DataType*>(vBuffer), static_cast<DataType*>(vBuffer) + vBufferLen);
        m_datas = m_compress(buffer);
        return *this;
    }

    Lzw& compresss(const std::string& vBuffer) {
        m_datas = m_compress(m_stringToVector(vBuffer));
        return *this;
    }

    Lzw& extractFile(const std::string& vFilePathName) {
        std::ifstream inputFile(vFilePathName, std::ios::binary);
        if (inputFile.is_open()) {
            std::vector<DataType> buffer((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
            inputFile.close();
            m_datas = m_extract(buffer);
        }
        return *this;
    }

    Lzw& extract(const std::vector<DataType>& vDatas) {
        m_datas = m_extract(vDatas);
        return *this;
    }

    Lzw& extract(void* vBuffer, size_t vBufferLen) {
        std::vector<DataType> buffer(static_cast<DataType*>(vBuffer), static_cast<DataType*>(vBuffer) + vBufferLen);
        m_datas = m_extract(buffer);
        return *this;
    }

    Lzw& extract(const std::string& vBuffer) {
        std::vector<DataType> buffer(vBuffer.begin(), vBuffer.end());
        m_datas = m_extract(buffer);
        return *this;
    }

    std::vector<DataType> getDatas() {
        return m_datas;
    }

    std::string getDatasToString() {
        return m_vectorToString(m_datas);
    }

    Lzw& save(const std::string& vFilePathName, bool vNoExcept = false) {
        std::ofstream file(vFilePathName, std::ios::binary);
        if (!file && !vNoExcept) {
            throw std::runtime_error("Impossible d'ouvrir le fichier.");
        }
        auto dataSize = static_cast<uint32_t>(m_datas.size());
        file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        file.write(reinterpret_cast<const char*>(m_datas.data()), m_datas.size());
        file.close();
        return *this;
    }

private:
    static std::vector<DataType> m_stringToVector(const std::string& str) {
        return {str.begin(), str.end()};
    }

    static std::string m_vectorToString(const std::vector<DataType>& vec) {
        return {vec.begin(), vec.end()};
    }

/*
*     PSEUDOCODE
1     Initialize table with single character strings
2     P = first input character
3     WHILE not end of input stream
4          C = next input character
5          IF P + C is in the string table
6            P = P + C
7          ELSE
8            output the code for P
9            add P + C to the string table
10           P = C
11         END WHILE
12    output code for P 
*/

    std::vector<DataType> m_compress(const std::vector<DataType>& vDatas) {
        std::vector<DataType> result;
        uint16_t code = 256U; 
        std::string p, c;
        p += static_cast<char>(vDatas.at(0));
        for (uint16_t idx = 1U; idx < vDatas.size(); ++idx) {
            c = static_cast<char>(vDatas.at(idx));
            auto pc = p + c;
            if (m_dicoComp.find(pc) != m_dicoComp.end()) {
                p = pc;
            } else {
                result.push_back(m_dicoComp.at(p));
                m_dicoComp[pc] = code++;
                p = c;
            }
        }
        result.push_back(m_dicoComp.at(p));
        return result;
    }

/*
*    PSEUDOCODE
1    Initialize table with single character strings
2    OLD = first input code
3    output translation of OLD
4    WHILE not end of input stream
5        NEW = next input code
6        IF NEW is not in the string table
7               S = translation of OLD
8               S = S + C
9       ELSE
10              S = translation of NEW
11       output S
12       C = first character of S
13       OLD + C to the string table
14       OLD = NEW
15   END WHILE
*/
    std::vector<DataType> m_extract(const std::vector<DataType>& vDatas) {
        std::vector<DataType> result;
        DataType lop = vDatas.at(0), nop;
        std::string p = m_dicoDeComp.at(lop);
        std::string c = p;
        result.push_back(p.at(0));
        uint16_t code = 256U;
        for (size_t idx = 1U; idx < vDatas.size(); ++idx) {
            nop = vDatas[idx];
            if (m_dicoDeComp.find(nop) == m_dicoDeComp.end()) {
                p = m_dicoDeComp.at(lop);
                p += c;
            } else {
                p = m_dicoDeComp.at(nop);
            }
            for (DataType ch : p) {
                result.push_back(ch);
            }
            c = p[0];
            m_dicoDeComp[code++] = m_dicoDeComp.at(lop) + c;
            lop = nop;
        }
        return result;
    }
};

}  // namespace comp
}  // namespace ez
