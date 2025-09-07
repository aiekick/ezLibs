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
#include "ezTile.hpp"
#include "../ezBinBuf.hpp"
#include "../ezMath.hpp"

namespace ez {
namespace geo {

class DemBin {
public:
    struct Datas {
        std::string latStr;
        std::string lonStr;
        int8_t lat;
        int8_t lon;
        tile<int16_t> tile;
    };

private:
    Datas m_datas;

public:
    bool load(const std::string& vName, const std::vector<uint8_t> vBytes) {
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
        m_datas.lat = parseDemCoordinate(m_datas.latStr);
        m_datas.lon = parseDemCoordinate(m_datas.lonStr);
        const auto side = m_computeSizeFromBufferSize(vBytes.size());
        const auto nLats = side;
        const auto nLons = side;
        ez::BinBuf binBuf;
        binBuf.setDatas(vBytes);
        size_t pos = 0;
        auto& tileDatas = m_datas.tile.getDatasRef();
        tileDatas.resize(nLats);
        for (uint16_t row = 0; row < nLats; ++row) {
            // read row
            auto& matRow = tileDatas.at(row);
            matRow.resize(nLons);
            binBuf.readArrayBE<int16_t>(pos, matRow.data(), matRow.size());
        }

        if (vMem.bytes.empty()) {
            return false;
        }
        auto& datas = getDatasRef();
        ez::BinBuf binBuf;
        binBuf.setDatas(vMem.bytes);
        size_t pos = 0;  // on saute la date
        std::string date(16, 0);
        binBuf.readArrayBE(pos, date.data(), date.size());
        // 01/01/2025 12/17
        auto arr = ez::str::splitStringToVector(date, '/');
        if (arr.size() == 4) {
            const auto resolutionLat = binBuf.readValueBE<uint32_t>(pos);
            const auto resolutionLon = binBuf.readValueBE<uint32_t>(pos);
            const auto lat = binBuf.readValueBE<float>(pos);
            const auto lon = binBuf.readValueBE<float>(pos);
            datas.nLats = binBuf.readValueBE<uint32_t>(pos);
            datas.nLons = binBuf.readValueBE<uint32_t>(pos);
            datas.ratio = static_cast<double>(datas.nLats) / static_cast<double>(datas.nLons);
            std::vector<int16_t> rowValues(datas.nLons);
            for (size_t row = 0; row < datas.nLats; ++row) {
                binBuf.readArrayBE<int16_t>(pos, rowValues.data(), rowValues.size());
                for (const auto& value : rowValues) {
                    datas.range.combine(value);
                }
                datas.matriceDatas.push_back(rowValues);
            }
            if (!datas.matriceDatas.empty()) {
                datas.valid = true;
                datas.loaded = true;
            }
        }
        return m_datas.tile.check();
    }

    bool save(const std::string& vFile) const { return false; }

    bool isValid() const { return m_datas.tile.isValid(); }
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