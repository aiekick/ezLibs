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

class hgt {
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
        if (vBytes.empty() && checkDemFileName(vName, m_datas.latStr, m_datas.lonStr)) {
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
        }
        return m_datas.tile.check();
    }

    bool save(std::vector<uint8_t>& voBytes) const {
        ez::BinBuf binBuf;
        const auto& tileDatas = m_datas.tile.getDatas();
        for (const auto& row : tileDatas) {
            binBuf.writeArrayBE<int16_t>(row.data(), row.size());
        }
        voBytes = binBuf.getDatas();    
        return true; 
    }

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