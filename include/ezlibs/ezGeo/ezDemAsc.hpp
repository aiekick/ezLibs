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

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
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
        if (!vBuffer.empty() && checkDemFileName(vName, m_datas.latStr, m_datas.lonStr)) {
            const auto lines = ez::str::splitStringToVector(vBuffer, '\n');
            size_t idx{};
            auto& tileDatas = m_datas.tile.getDatasRef();
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
                        std::vector<int16_t> rowValues(values.size());
                        for (size_t i = 0U; i < values.size(); ++i) {
                            const auto& valueString = values.at(i);
                            rowValues[i] = static_cast<int16_t>(strtol(valueString.c_str(), nullptr, 10));
                        }
                        tileDatas.push_back(rowValues);
                    }
                } else {
                    break;
                }
                ++idx;
            }
        }
        return m_datas.tile.check();
    }

    bool save(std::string& voBuffer) const {
        std::stringstream ss;
        ss << "nrows " << m_datas.nLats << "\n";
        ss << "ncols " << m_datas.nLons << "\n";
        ss << "xllcorner " << m_datas.xllcorner << "\n";
        ss << "yllcorner " << m_datas.yllcorner << "\n";
        ss << "cellsize " << m_datas.cellsize << "\n";
        ss << "NODATA_value " << m_datas.NODATA_value << std::endl;  // cause a flush
        auto& tileDatas = m_datas.tile.getDatas();
        for (const auto& row : tileDatas) {
            for (const auto& col : row) {
                ss << col << " ";
            }
            ss << std::endl;  // cause a flush
        }
        voBuffer = ss.str();
        return true;
    }

    bool isValid() const { return m_datas.tile.isValid(); }
    const Datas& getDatas() const { return m_datas; }
    Datas& getDatasRef() { return m_datas; }
};

}  // namespace geo
}  // namespace ez