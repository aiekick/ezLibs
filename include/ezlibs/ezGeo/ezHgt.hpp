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

// ezGeo is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <cmath>>
#include <vector>
#include <string>
#include <cstdint>

#include "ezGeo.hpp"
#include "../ezBinBuf.hpp"
#include "../ezMath.hpp"

namespace ez {
namespace geo {

class hgt {
public:
    struct Datas {
        std::string latStr;
        std::string lonStr;
        int8_t lat;
        int8_t lon;
        uint16_t nLats{};
        uint16_t nLons{};
        std::vector<std::vector<int16_t>> matrice;
        ez::range<int16_t> range{};
        bool valid{};
    };

private:
    Datas m_datas;

public:
    bool load(const std::string& vName, const std::vector<uint8_t> vBytes) {
        m_datas.valid = false;
        if (vBytes.empty()) {
#ifdef EZ_TOOLS_LOG
            LogVarError(u8R"(Bytes are empty)");
#endif
            return false;
        }
        if (!checkDemFileName(vName, m_datas.latStr, m_datas.lonStr)) {
#ifdef EZ_TOOLS_LOG
            LogVarError(u8R"(Name "%s" have not the good format ex: N01E006)", vName.c_str());
#endif
            return false;
        }
        m_datas.nLats = m_datas.nLons = m_computeSizeFromBufferSize(vBytes.size());
        ez::BinBuf binBuf;
        binBuf.setDatas(vBytes);
        size_t pos = 0;
        m_datas.matrice.resize(m_datas.nLons);
        for (uint16_t row = 0; row < m_datas.nLats; ++row) {
            // read row
            auto& matRow = m_datas.matrice.at(row);
            matRow.resize(m_datas.nLats);
            binBuf.readArrayBE<int16_t>(pos, matRow.data(), matRow.size());
            // re range
            for (const auto& value : matRow) {
                m_datas.range.combine(value);
            }
        }
        m_datas.valid = !m_datas.matrice.empty();
        return isValid();
    }

    bool save(const std::string& vFile) const { return false; }

    bool isValid() const { return m_datas.valid; }
    const Datas& getDatas() const { return m_datas; }
    Datas& getDatasRef() { return m_datas; }

    private:
    int16_t m_computeSizeFromBufferSize(const size_t vBufferSize) {
        auto side = static_cast<uint32_t>(std::sqrt(static_cast<double>(vBufferSize) / 2.0));
        if (side < 3601) {
            return 1201;
        } else {
            return 3601;
        }
    }
};

}  // namespace geo
}  // namespace ez