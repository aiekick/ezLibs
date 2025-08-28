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
#include <string>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <utility>  // std::pair

#include "../ezMath.hpp"

namespace ez {
namespace geo {

// WGS84 spherical earth radius in meters
constexpr double EARTH_RADIUS = 6378137.0;

// Convert Mercator coordinates (x,y in meters) -> (lon,lat) in degrees
inline ez::dvec2 fromMercatorMetersToDegrees(const ez::dvec2& vPointMeters) {
    ez::dvec2 ret;
    double lonRad = vPointMeters.x / EARTH_RADIUS;
    double latRad = 2.0 * std::atan(std::exp(vPointMeters.y / EARTH_RADIUS)) - M_PI / 2.0;
    ret.x = lonRad * 180.0 / M_PI;
    ret.y = latRad * 180.0 / M_PI;
    return ret;
}

// Convert (lon,lat) in degrees -> Mercator coordinates (x,y) in meters
inline ez::dvec2 fromDegressToMercatorMeters(const ez::dvec2& vPointDegree) {
    ez::dvec2 ret{vPointDegree};
    // Clamp latitude to avoid infinities at the poles
    const double maxLat = 89.9999;
    if (ret.y > maxLat)
        ret.y = maxLat;
    if (ret.y < -maxLat)
        ret.y = -maxLat;

    double lonRad = ret.x * M_PI / 180.0;
    double latRad = ret.y * M_PI / 180.0;
    ret.x = EARTH_RADIUS * lonRad;
    ret.y = EARTH_RADIUS * std::log(std::tan(M_PI / 4.0 + latRad / 2.0));
    return ret;
}

// Convert Mercator Y (meters) -> unrolled Y (linearized meters)
inline double mercatorYToUnrolledY(double yMerc) {
    // 1) Convert Mercator Y -> latitude φ in radians
    double phi = 2.0 * std::atan(std::exp(yMerc / EARTH_RADIUS)) - M_PI / 2.0;
    // 2) Unroll: linearized Y = R * φ
    return EARTH_RADIUS * phi;
}

// Rescale latitude axis for WGS84 approximated ellipse
inline double unscaleWGS84Yaxis(double vLatDeg, double vRefDeg = 45.0) {
    const double phi0 = vRefDeg * M_PI / 180.0;
    const double k = 111132.0 / (111320.0 * std::cos(phi0));  // ≈0.9983 / cos(phi0)
    return vLatDeg * k;
}

/*
 * Parse DEM coordinate component like "S01", "N10", "E001", "W112".
 * Returns the signed numeric value.
 * Example:
 *   "N10" -> +10
 *   "S10" -> -10
 *   "E001" -> +1
 *   "W112" -> -112
 */
inline int32_t parseDemCoordinate(const std::string& str) {
    const int value = std::stoi(str.substr(1));
    char c = str.at(0);
    if (c == 'S' || c == 's' || c == 'W' || c == 'w') {
        return -value;
    }
    return value;
}

/*
 * Check if a DEM filename is valid.
 * Expected format: N00E000 (7 chars total)
 *   - N|S followed by 2 digits
 *   - E|W followed by 3 digits
 * Returns true if valid, also fills vOutCx and vOutCy with longitude/latitude codes.
 */
inline bool checkDemFileName(const std::string& vFileName, std::string& vOutCx, std::string& vOutCy) {
    if (vFileName.size() != 7U)
        return false;
    try {
        bool ret = true;
        const auto& cy = vFileName.substr(0, 3);
        const char ccy = cy.at(0);
        if (ccy == 'S' || ccy == 's' || ccy == 'N' || ccy == 'n') {
            const auto& cy_num = cy.substr(1);
            if (!ez::isInteger(cy_num))
                ret = false;
        } else {
            ret = false;
        }
        const auto& cx = vFileName.substr(3);
        const char ccx = cx.at(0);
        if (ccx == 'E' || ccx == 'e' || ccx == 'W' || ccx == 'w') {
            const auto& cx_num = cx.substr(1);
            if (!ez::isInteger(cx_num))
                ret = false;
        } else {
            ret = false;
        }
        if (ret) {
            vOutCx = cx;
            vOutCy = cy;
        }
        return ret;
    } catch (std::exception& ex) {
        LogVarError("Error : %s", ex.what());
    }
    return false;
}

/*
 * DMS class:
 * Stores coordinates in Degrees, Minutes, Seconds and a cardinal letter.
 * Examples of valid inputs: "48°51'24\"N", "45 30 15 S"
 */
struct dms {
    float deg{};
    float min{};
    float sec{};
    char letter{' '};
    bool valid{false};

    dms() = default;

    // Construct from a DMS string like "45°30'15\"N" or "45 30 15 N"
    explicit dms(const std::string& vDms) { valid = parse(vDms); }

    // Construct from a decimal angle, with flag isLat (true for latitude, false for longitude)
    explicit dms(double vAngle, bool vIsLat) {
        fromAngle(vAngle, vIsLat);
        valid = true;
    }

    // Basic setters
    dms& setDeg(float vDeg) {
        deg = vDeg;
        return *this;
    }
    dms& setMin(float vMin) {
        min = vMin;
        return *this;
    }
    dms& setSec(float vSec) {
        sec = vSec;
        return *this;
    }
    dms& setLetter(char vLetter) {
        letter = vLetter;
        return *this;
    }

    // offsets
    // Adds degrees, pushes fractional part to minutes, then wraps if lat/lon.
    dms& offsetDeg(float vDeg) {
        // 0) First, purge any existing fractional degrees -> push into minutes
        const float dIntExisting  = ez::floor(deg);
        const float dFracExisting = deg - dIntExisting;
        deg = dIntExisting;
        if (dFracExisting != 0.0f) {
            // push to minutes as fractional part
            sec += (dFracExisting * 60.0f) * 60.0f; // deg.frac * 60 = min.frac; *60 again -> seconds
        }

        // 1) Split input degrees: integer to deg, fractional cascades to min/sec
        const float dInt  = ez::floor(vDeg);
        const float dFrac = vDeg - dInt;
        deg += dInt;

        if (dFrac != 0.0f) {
            // dFrac degrees -> minutes.frac = dFrac * 60 -> seconds addition
            sec += (dFrac * 60.0f) * 60.0f;
        }

        // 2) Normalize seconds -> minutes
        const float carryMin = ez::floor(sec / 60.0f);
        sec -= carryMin * 60.0f;
        min += carryMin;

        // 3) Ensure 'min' is integer: push fractional residue into seconds
        const float minFrac = min - ez::floor(min);
        if (minFrac != 0.0f) {
            min = ez::floor(min);
            sec += minFrac * 60.0f;

            // Re-normalize seconds
            const float carryMin2 = ez::floor(sec / 60.0f);
            sec -= carryMin2 * 60.0f;
            min += carryMin2;
        }

        // 4) Carry minutes into degrees
        const float carryDeg = ez::floor(min / 60.0f);
        min -= carryDeg * 60.0f;
        deg += carryDeg;

        // 5) Optional wrap
        if (isLat()) {
            deg = ez::mod(deg, 90.0f);
        } else if (isLon()) {
            deg = ez::mod(deg, 180.0f);
        }
        return *this;
    }

    // Adds minutes, normalizes into [0,60), carries into degrees, wraps if needed.
    dms& offsetMin(float vMin) {
        // 1) Split input minutes: integer part goes to minutes, fractional to seconds
        const float mInt  = ez::floor(vMin);
        const float mFrac = vMin - mInt;

        // 2) Add integer minutes
        min += mInt;

        // 3) Push fractional minutes into seconds
        if (mFrac != 0.0f) {
            sec += mFrac * 60.0f;
        }

        // 4) Normalize seconds -> minutes
        const float carryMin = ez::floor(sec / 60.0f);
        sec -= carryMin * 60.0f;
        min += carryMin;

        // 5) Ensure 'min' is integer: push any fractional residue into seconds
        const float minFrac = min - ez::floor(min);
        if (minFrac != 0.0f) {
            min = ez::floor(min);
            sec += minFrac * 60.0f;

            // Re-normalize seconds since we just added some
            const float carryMin2 = ez::floor(sec / 60.0f);
            sec -= carryMin2 * 60.0f;
            min += carryMin2;
        }

        // 6) Carry minutes into degrees (keep min in [0,60) and integer)
        const float carryDeg = ez::floor(min / 60.0f);
        min -= carryDeg * 60.0f;
        deg += carryDeg;

        // 7) Optional wrap
        if (isLat()) {
            deg = ez::mod(deg, 90.0f);
        } else if (isLon()) {
            deg = ez::mod(deg, 180.0f);
        }
        return *this;
    }

    // Adds seconds, normalizes into [0,60), carries into minutes (then degrees via offsetMin).
    dms& offsetSec(float vSec) {
        // 1) Add seconds
        sec += vSec;

        // 2) Normalize seconds into [0,60) and carry into minutes (floor works for negatives)
        const float carryMin = ez::floor(sec / 60.0f);
        sec -= carryMin * 60.0f;
        min += carryMin;

        // 3) Ensure 'min' is an integer: push any fractional minutes into seconds
        const float minFrac = min - ez::floor(min);
        if (minFrac != 0.0f) {
            min = ez::floor(min);
            sec += minFrac * 60.0f;

            // Re-normalize seconds because we just added some
            const float carryMin2 = ez::floor(sec / 60.0f);
            sec -= carryMin2 * 60.0f;
            min += carryMin2;
        }

        // 4) Carry minutes into degrees, keep min in [0,60) and integer
        const float carryDeg = ez::floor(min / 60.0f);
        min -= carryDeg * 60.0f;
        deg += carryDeg;

        // 5) Optional wrap on degrees (no hemisphere flip)
        if (isLat()) {
            deg = ez::mod(deg, 90.0f);
        } else if (isLon()) {
            deg = ez::mod(deg, 180.0f);
        }
        return *this;
    }

    // Type checks
    bool isLat() const { return (letter == 'N') || (letter == 'S'); }
    bool isLon() const { return (letter == 'E') || (letter == 'W'); }
    bool isValid() const { return valid; }

    // Normalize DMS values by carrying overflow of seconds and minutes
    static void normalizeDMS(double& D, double& M, double& S) {
        if (S >= 59.9999995) {
            S = 0.0;
            M += 1.0;
        }
        if (M >= 60.0) {
            M = 0.0;
            D += 1.0;
        }
    }

    // Parse a DMS string into components
    bool parse(const std::string& vDms) {
        std::string str;
        for (char c : vDms) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (std::isdigit(uc) || uc == '.' || uc == '-' || uc == '+' || uc == ' ') {
                str += static_cast<char>(uc);
            } else if (std::isalpha(uc)) {
                letter = static_cast<char>(std::toupper(uc));
            } else {
                str += ' ';
            }
        }
        std::istringstream iss(str);
        std::vector<double> vals;
        for (double x; iss >> x;) {
            vals.push_back(x);
            if (vals.size() == 3) {
                break;
            }
        }
        if (vals.empty()) {
            return false;
        }
        double d = 0.0, m = 0.0, s = 0.0;
        const double sign = (vals[0] < 0.0) ? -1.0 : 1.0;
        if (letter == ' ' && sign < 0.0){
            letter = '-'; // special case for parse("-12.5")
        }
        if (vals.size() == 1) {
            // Decimal degrees -> convert to DMS
            const double ad = std::fabs(vals[0]);
            d = std::floor(ad + 1e-12);
            const double rem_min = (ad - d) * 60.0;
            m = std::floor(rem_min + 1e-12);
            s = (rem_min - m) * 60.0;
            normalizeDMS(d, m, s);
        } else if (vals.size() == 2) {
            if (vals[1] < 0.0) {
                return false;
            }
            // Degrees + minutes
            d = std::floor(std::fabs(vals[0]) + 1e-12);
            m = std::fabs(vals[1]);
            if (!(m >= 0.0 && m < 60.0)) {
                return false;
            }
            const double mi = std::floor(m + 1e-12);
            s = (m - mi) * 60.0;
            m = mi;
            normalizeDMS(d, m, s);
        } else {
            if (vals[1] < 0.0) {
                return false;
            }
            if (vals[2] < 0.0) {
                return false;
            }
            // Degrees + minutes + seconds
            d = std::floor(std::fabs(vals[0]) + 1e-12);
            m = std::fabs(vals[1]);
            s = std::fabs(vals[2]);
            if (!(m >= 0.0 && m < 60.0)) {
                return false;
            }
            if (!(s >= 0.0 && s < 60.0)) {
                return false;
            }
            normalizeDMS(d, m, s);
        }
        deg = static_cast<float>(d);
        min = static_cast<float>(m);
        sec = static_cast<float>(s);
        return true;
    }

    // Convert DMS -> decimal angle
    double toAngle() const {
        return getSign() * (deg + min / 60.0 + sec / 3600.0);
    }

    // get the sign
    double getSign() const {return (letter == 'S' || letter == 'W' || letter == '-') ? -1.0 : 1.0;}

    // Convert DMS -> string
    std::string toDmsString() const {
        std::stringstream ss;
        ss << deg << "°" << min << "'" << sec << "\"" << letter;
        return ss.str();
    }

    // Convert decimal angle -> DMS + letter
    void fromAngle(const double vAngle, const bool vIsLat) {
        const double sign = (vAngle < 0.0) ? -1.0 : 1.0;
        const double a = std::fabs(vAngle);
        deg = static_cast<float>(std::floor(a));
        const double frac = (a - deg) * 60.0;
        min = static_cast<float>(std::floor(frac));
        sec = static_cast<float>((frac - min) * 60.0);
        if (vIsLat) {
            letter = (sign < 0.0) ? 'S' : 'N';
        } else {
            letter = (sign < 0.0) ? 'W' : 'E';
        }
    }
};

}  // namespace geo
}  // namespace ez
