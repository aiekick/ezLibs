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

#include <array>
#include <cmath>
#include <vector>
#include <string>
#include <cstdint>

#include "ezGeo.hpp"
#include "ezTile.hpp"
#include "../ezStackString.hpp"
#include "../ezBinBuf.hpp"
#include "../ezMath.hpp"

namespace ez {
namespace geo {

class DemBin {
public:
    struct Datas {
        std::string latStr;
        std::string lonStr;
        uint32_t nLats{};
        uint32_t nLons{};
        uint32_t resLat{};
        uint32_t resLon{};
        int16_t lat{};
        int16_t lon{};
        float fLat{};
        float fLon{};
        std::array<char, 16> date{'0', '1', '/', '0', '1', '/', '1', '9', '7', '0', ' ', '0', '0', '/', '0', '0'};
        tile<int16_t> tile;
    };

private:
    Datas m_datas;

public:
    bool load(const std::string& vName, const std::vector<uint8_t> vBytes) {
        if (!vBytes.empty() && checkDemFileName(vName, m_datas.latStr, m_datas.lonStr)) {
            m_datas.lat = parseDemCoordinate(m_datas.latStr);
            m_datas.lon = parseDemCoordinate(m_datas.lonStr);
            ez::BinBuf binBuf;
            binBuf.setDatas(vBytes);
            size_t pos = 0;
            binBuf.readArrayBE(pos, m_datas.date.data(), m_datas.date.size());
            std::string date(m_datas.date.data(), m_datas.date.size());
            auto arr = ez::str::splitStringToVector(date, '/');
            if (arr.size() == 4) {
                auto& tileDatas = m_datas.tile.getDatasRef();
                m_datas.resLat = binBuf.readValueBE<uint32_t>(pos);
                m_datas.resLon = binBuf.readValueBE<uint32_t>(pos);
                m_datas.fLat = binBuf.readValueBE<float>(pos);
                m_datas.fLon = binBuf.readValueBE<float>(pos);
                m_datas.nLats = binBuf.readValueBE<uint32_t>(pos);
                m_datas.nLons = binBuf.readValueBE<uint32_t>(pos);
                tileDatas.resize(m_datas.nLats);
                std::vector<int16_t> rowValues(m_datas.nLons);
                for (size_t row = 0; row < m_datas.nLats; ++row) {
                    binBuf.readArrayBE<int16_t>(pos, rowValues.data(), rowValues.size());
                    tileDatas[row] = rowValues;
                }
            }
        }
        return m_datas.tile.check();
    }

    bool save(std::vector<uint8_t>& voBytes) const {
        ez::BinBuf binBuf;
        std::string date(m_datas.date.begin(), m_datas.date.end());
        binBuf.writeArrayBE(date.data(), date.size());
        binBuf.writeValueBE<uint32_t>(m_datas.resLat);
        binBuf.writeValueBE<uint32_t>(m_datas.resLon);
        binBuf.writeValueBE<float>(m_datas.fLat);
        binBuf.writeValueBE<float>(m_datas.fLon);
        binBuf.writeValueBE<uint32_t>(m_datas.nLats);
        binBuf.writeValueBE<uint32_t>(m_datas.nLons);
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
};

}  // namespace geo
}  // namespace ez