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
#include "../ezMath.hpp"

namespace ez {
namespace geo {

class DemAsc {
public:
    struct Datas {
        double xllcorner{};
        double yllcorner{};
        uint32_t cellsize{};
        int32_t NODATA_value{};
        std::string latStr;
        std::string lonStr;
        uint32_t nLats{};
        uint32_t nLons{};
        int8_t lat{};
        int8_t lon{};
        tile<int16_t> tile;
    };

private:
    Datas m_datas;

public:
    bool load(const std::string& vName, const std::string& vBuffer) {
        if (vBuffer.empty()) {
#ifdef EZ_TOOLS_LOG
            LogVarError(u8R"(buffer is empty)");
#endif
            return false;
        }
        if (!checkDemFileName(vName, m_datas.latStr, m_datas.lonStr)) {
#ifdef EZ_TOOLS_LOG
            LogVarError(u8R"(Name "%s" have not the good format ex: N01E006)", vName.c_str());
#endif
            return false;
        }
        const auto lines = ez::str::splitStringToVector(vBuffer, '\n');
        size_t idx{};
        for (const auto& line : lines) {
            const auto values = ez::str::splitStringToVector(line, ' ');
            if (!values.empty()) {
                if (idx == 0 && values.at(0) == "nrows") {
                    m_datas.nLats = strtoul(values.at(1).c_str(), nullptr, 10);
                } else if (idx == 1 && values.at(0) == "ncols") {
                    m_datas.nLons = strtoul(values.at(1).c_str(), nullptr, 10);
                } else if (idx == 2 && values.at(0) == "xllcorner") {
                    m_datas.xllcorner = strtod(values.at(1).c_str(), nullptr);
                } else if (idx == 3 && values.at(0) == "yllcorner") {
                    m_datas.yllcorner = strtod(values.at(1).c_str(), nullptr);
                } else if (idx == 4 && values.at(0) == "cellsize") {
                    m_datas.cellsize = strtoul(values.at(1).c_str(), nullptr, 10);
                } else if (idx == 5 && values.at(0) == "NODATA_value") {
                    m_datas.NODATA_value = strtol(values.at(1).c_str(), nullptr, 10);
                } else {
                    if (idx <= 5) {
                        break;
                    }
                    auto& tileDatas = m_datas.tile.getDatasRef();
                    tileDatas.resize(values.size());
                    std::vector<int16_t> rowValues(values.size());
                    for (size_t idx = 0U; idx < values.size(); ++idx) {
                        const auto& valueString = values.at(idx);
                        const int16_t valueInt16 = static_cast<int16_t>(strtol(valueString.c_str(), nullptr, 10));
                        rowValues[idx] = valueInt16;
                    }
                    if (rowValues.size() > m_datas.nLons) {
                        std::cout << "Warnings : Line " << idx << " have more value(" << rowValues.size() << ") than ncols(" << datas.nLons << ")" << std::endl;
                    }
                    datas.matriceDatas.push_back(rowValues);
                }
            } else {
                break;
            }
            ++idx;
        }
        if (datas.matriceDatas.size() > datas.nLats) {
            std::cout << "Warnings : Datas have more row(" << datas.matriceDatas.size() << ") than nrows(" << datas.nLats << ")" << std::endl;
        }
        if (!datas.matriceDatas.empty()) {
            datas.valid = true;
            datas.loaded = true;
        }
        if (!datas.valid) {
            std::cout << "Tile file " << datas.filePathName << "is invalid or not a mnt file" << std::endl;
        }

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