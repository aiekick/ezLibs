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

// ezTile is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <cmath>
#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <type_traits>

#include "ezGeo.hpp"
#include "../ezMath.hpp"

namespace ez {

// --------------------------------------------------------------------------------------
// Convention used here (critical for consistency):
// - vec2<geo::dms> stores: x = latitude, y = longitude
// - Datas container is addressed as m_datas[latIndex][lonIndex]
//   meaning "rows = latitudes, cols = longitudes"
// - Grid spacing is 1.0 degree in both latitude and longitude.
// --------------------------------------------------------------------------------------

template <>
struct vec2<geo::dms> {
    geo::dms x{};  // latitude
    geo::dms y{};  // longitude

    vec2() = default;

    // Constructor: (lat, lon) in decimal degrees
    explicit vec2(double vLat, double vLon) : x(geo::dms(vLat, true)), y(geo::dms(vLon, false)) {}

    // Convert to angles in decimal degrees (x = latDeg, y = lonDeg)
    dvec2 toAngle() const { return dvec2(x.toAngle(), y.toAngle()); }
};

typedef vec2<geo::dms> dmsCoord;

namespace geo {

/*
tile is a container of geo datas built from a coord and a matrix buffer.
Coordinates are (lat, lon) mapped to (x, y) as stated above:
- x = latitude, y = longitude
- m_datas is indexed as m_datas[lat][lon]
*/
template <typename TDATAS>
class tile {
public:
    typedef std::vector<std::vector<TDATAS>> DatasContainer;

private:
    DatasContainer m_datas;
    dmsCoord m_min;      // min latitude (x) and min longitude (y) in DMS
    dmsCoord m_max;      // max latitude (x) and max longitude (y) in DMS
    uint32_t m_nLats{};  // number of rows (latitude samples)
    uint32_t m_nLons{};  // number of cols (longitude samples)
    ez::range<TDATAS> m_range;
    bool m_valid{false};

public:
    tile() = default;

    // vMin is the (lat, lon) of the minimal corner (in degrees)
    // m_datas must be a rectangular matrix: rows = lats, cols = lons
    tile(const DatasContainer& vDatas, const dmsCoord& vMin) : m_datas(vDatas), m_min(vMin) { m_valid = check(); }

    bool isValid() const { return m_valid; }

    // Counts
    uint32_t getNLats() const { return m_nLats; }
    uint32_t getNLons() const { return m_nLons; }

    // Min/Max DMS coords
    const dmsCoord& getMinDms() const { return m_min; }
    const dmsCoord& getMaxDms() const { return m_max; }

    // range
    ez::range<TDATAS> getRange() const { return m_range; }

    DatasContainer& getDatasRef() { return m_datas; }
    const DatasContainer& getDatas() const { return m_datas; }

    // -----------------------------------------------------------------------------
    // Discrete access by indices
    // vCoord = {lonIndex, latIndex}
    // Returns the raw TDATAS sample (no int32_terpolation).
    // -----------------------------------------------------------------------------
    bool getValue(const uvec2& vCoord, TDATAS& voValue) const {
        if (!m_valid) {
            return false;
        }

        const uint32_t lon = vCoord.x;
        const uint32_t lat = vCoord.y;

        if (lat >= m_nLats || lon >= m_nLons) {
            return false;
        }

        voValue = m_datas.at(static_cast<size_t>(lat)).at(static_cast<size_t>(lon));
        return true;
    }


    // -----------------------------------------------------------------------------
    // Continuous access by geo coordinate (lat, lon in DMS)
    // Bilinear int32_terpolation with optional wrapping on lat/lon.
    // Always outputs a double in voValue (no int32_teger truncation).
    // -----------------------------------------------------------------------------
    bool getValue(const dmsCoord& vCoord, double& voValue, bool vWrapLat = false, bool vWrapLon = false) const {
        if (!m_valid) {
            return false;
        }

        // Convert DMS to decimal degrees
        const double latMin = m_min.x.toAngle();
        const double lonMin = m_min.y.toAngle();
        const double latMax = m_max.x.toAngle();
        const double lonMax = m_max.y.toAngle();

        double lat = vCoord.x.toAngle();  // latitude
        double lon = vCoord.y.toAngle();  // longitude

        const double latSpan = static_cast<double>(m_nLats);
        const double lonSpan = static_cast<double>(m_nLons);

        // Optional wrapping int32_to tile range [min, min+span)
        if (vWrapLat) {
            if (latSpan <= 0.0) {
                return false;
            }
            double d = std::fmod(lat - latMin, latSpan);
            if (d < 0.0) {
                d += latSpan;
            }
            lat = latMin + d;
        }
        if (vWrapLon) {
            if (lonSpan <= 0.0) {
                return false;
            }
            double d = std::fmod(lon - lonMin, lonSpan);
            if (d < 0.0) {
                d += lonSpan;
            }
            lon = lonMin + d;
        }

        // Strict bounds if no wrap
        if (!vWrapLat && (lat < latMin || lat > latMax)) {
            return false;
        }
        if (!vWrapLon && (lon < lonMin || lon > lonMax)) {
            return false;
        }

        // Fractional indices in the grid (1 degree per step)
        double fLat = (lat - latMin);
        double fLon = (lon - lonMin);

        // Clamp int32_to [0, n-1] to keep indices valid
        if (fLat < 0.0) {
            fLat = 0.0;
        }
        if (fLon < 0.0) {
            fLon = 0.0;
        }
        if (fLat > static_cast<double>(m_nLats - 1)) {
            fLat = static_cast<double>(m_nLats - 1);
        }
        if (fLon > static_cast<double>(m_nLons - 1)) {
            fLon = static_cast<double>(m_nLons - 1);
        }

        const int32_t iLat = static_cast<int32_t>(std::floor(fLat));
        const int32_t iLon = static_cast<int32_t>(std::floor(fLon));
        const double tLat = fLat - static_cast<double>(iLat);
        const double tLon = fLon - static_cast<double>(iLon);

        // Helper to wrap/clamp indices according to flags and sizes
        auto wrapOrClamp = [](int32_t idx, int32_t size, bool doWrap) -> uint32_t {
            if (doWrap) {
                if (size <= 0) {
                    return 0u;
                }
                int32_t m = idx % size;
                if (m < 0) {
                    m += size;
                }
                return static_cast<uint32_t>(m);
            } else {
                if (idx < 0) {
                    idx = 0;
                }
                if (idx >= size) {
                    idx = size - 1;
                }
                return static_cast<uint32_t>(idx);
            }
        };

        // Compute neighbor indices (with wrap if requested)
        const uint32_t iLat0 = wrapOrClamp(iLat, static_cast<int32_t>(m_nLats), vWrapLat);
        const uint32_t iLat1 = wrapOrClamp(iLat + 1, static_cast<int32_t>(m_nLats), vWrapLat);
        const uint32_t iLon0 = wrapOrClamp(iLon, static_cast<int32_t>(m_nLons), vWrapLon);
        const uint32_t iLon1 = wrapOrClamp(iLon + 1, static_cast<int32_t>(m_nLons), vWrapLon);

        // If no neighbor and no wrap requested, fallback to nearest neighbor
        if ((!vWrapLat && (iLat + 1 >= static_cast<int32_t>(m_nLats))) || (!vWrapLon && (iLon + 1 >= static_cast<int32_t>(m_nLons)))) {
            TDATAS nearest{};
            if (!getValue(uvec2(iLon0, iLat0), nearest)) {
                return false;
            }
            voValue = static_cast<double>(nearest);
            return true;
        }

        // Fetch the 4 corners (order: v00=(lat0,lon0), v10=(lat1,lon0), v01=(lat0,lon1), v11=(lat1,lon1))
        TDATAS v00{};
        TDATAS v10{};
        TDATAS v01{};
        TDATAS v11{};
        bool ok = true;
        ok &= getValue(uvec2(iLon0, iLat0), v00);
        ok &= getValue(uvec2(iLon0, iLat1), v10);
        ok &= getValue(uvec2(iLon1, iLat0), v01);
        ok &= getValue(uvec2(iLon1, iLat1), v11);
        if (!ok) {
            return false;
        }

        // Bilinear int32_terpolation (lat vertically, lon horizontally)
        const double d00 = static_cast<double>(v00);
        const double d10 = static_cast<double>(v10);
        const double d01 = static_cast<double>(v01);
        const double d11 = static_cast<double>(v11);

        const double a = d00 * (1.0 - tLat) + d10 * tLat;  // along latitude
        const double b = d01 * (1.0 - tLat) + d11 * tLat;  // along latitude
        const double d = a * (1.0 - tLon) + b * tLon;      // along longitude

        voValue = d;
        return true;
    }

    // Validate the matrix, set sizes and deduce m_max from m_min and dimensions.
    bool check() {
        if (m_datas.empty()) {
#ifdef EZ_TOOLS_LOG
            LogVarError(u8R"(tile: data matrix is empty)");
#endif
            return false;
        }

        m_range = {};

        // rows = latitudes
        m_nLats = static_cast<uint32_t>(m_datas.size());

        // ensure rectangular matrix and compute m_nLons
        m_nLons = 0u;
        for (const auto& row : m_datas) {
            if (row.empty()) {
#ifdef EZ_TOOLS_LOG
                LogVarError(u8R"(tile: a row is empty)");
#endif
                return false;
            }
            if (m_nLons == 0u) {
                m_nLons = static_cast<uint32_t>(row.size());
            } else if (m_nLons != static_cast<uint32_t>(row.size())) {
#ifdef EZ_TOOLS_LOG
                LogVarError(u8R"(tile: non-rectangular matrix (row sizes differ))");
#endif
                return false;
            }
            for (const auto& v : row) {
                m_range.combine(v);
            }
        }

        // Compute max bounds from min + extents (1 degree per step)
        m_max = m_min;
        // x = latitude, y = longitude
        m_max.x.offsetDeg(static_cast<int32_t>(m_nLats));
        m_max.y.offsetDeg(static_cast<int32_t>(m_nLons));

        return true;
    }
};

}  // namespace geo
}  // namespace ez
