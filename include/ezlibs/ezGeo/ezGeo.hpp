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
#include <utility>  // std::pair

#include "../ezVec2.hpp"

namespace ez {
namespace geo {

constexpr double EARTH_RADIUS = 6378137.0;  // rayon sphérique WGS84 en mètres

// Coordonnées Mercator (x,y en mètres) -> (lon, lat) en degrés
inline ez::dvec2 fromMercatorMetersToDegrees(const ez::dvec2& vPointMeters) {
    ez::dvec2 ret;
    double lonRad = vPointMeters.x / EARTH_RADIUS;
    double latRad = 2.0 * std::atan(std::exp(vPointMeters.y / EARTH_RADIUS)) - M_PI / 2.0;
    ret.x = lonRad * 180.0 / M_PI;
    ret.y = latRad * 180.0 / M_PI;
    return ret;
}

// lon, lat en degrés -> coord. Mercator (x, y) en mètres
inline ez::dvec2 fromDegressToMercatorMeters(const ez::dvec2& vPointDegree) {
    ez::dvec2 ret{vPointDegree};
    // clamp latitude pour éviter les infinis aux pôles
    const double maxLat = 89.9999;
    if (ret.y > maxLat) {
        ret.y = maxLat;
    }
    if (ret.y < -maxLat) {
        ret.y = -maxLat;
    }
    double lonRad = ret.x * M_PI / 180.0;
    double latRad = ret.y * M_PI / 180.0;
    ret.x = EARTH_RADIUS * lonRad;
    ret.y = EARTH_RADIUS * std::log(std::tan(M_PI / 4.0 + latRad / 2.0));
    return ret;
}

// Mercator Y (m) -> déroulé Y (m)
inline double mercatorYToUnrolledY(double yMerc) {
    // 1) Mercator Y (m) -> latitude φ (rad)
    double phi = 2.0 * std::atan(std::exp(yMerc / EARTH_RADIUS)) - M_PI / 2.0;
    // 2) Déroulage : Y linéaire en mètres
    return EARTH_RADIUS * phi;
}

inline double unscaleWGS84Yaxis(double vLatDeg, double vRefDeg = 45.0) {
    const double phi0 = vRefDeg * M_PI / 180.0;
    const double k = 111132.0 / (111320.0 * std::cos(phi0));  // ~0.9983 / cos(phi0)
    return vLatDeg * k;
    //return vLatDeg * std::cos(phi0);
}

/*
* will parse coord one by one like S01 N10 E001 W112
* and for each return the number after the char in signed offset
* we consider than str is a valid coordinate
+1,-1       +1,+1
     NW | NE
     ---|---
     SW | SE
-1,-1       -1,+1
*/
inline int32_t parseDemCoordinate(const std::string& str) {
    const int value = std::stoi(str.substr(1));
    char c = str.at(0);
    if (c == 'S' || c == 's' || c == 'W' || c == 'w') {
        return -value;
    }
    /* 
    if (c > 'a') {
        c -= ('a' - 'A'); // convert in UPPER CASE
    }
    if (c > 'n') { // if > 'n' so its like S or W
        return -value;
    }    
    */
    return value;
}

/*
will check DEM file name.
ensure the format is N00E000 N|S, number on 2 char, E|W, number on 3 char
return : 
  - true if valid
  - vOutCx : who is the longitude (number of E|W)
  - vOutCy : who is the latitiude  (number of S|N)
*/
inline bool checkDemFileName(const std::string& vFileName, std::string& vOutCx, std::string& vOutCy) {
    if (vFileName.size() != 7U) {
        return false;
    }
    try {
        bool ret = true;
        const auto& cy = vFileName.substr(0, 3);
        const char ccy = cy.at(0);
        if (ccy == 'S' || ccy == 's' || ccy == 'N' || ccy == 'n') {
            const auto& cy_num = cy.substr(1);
            if (!ez::isInteger(cy_num)) {
                ret = false;
            }
        } else {
            ret = false;
        }
        const auto& cx = vFileName.substr(3);
        const char ccx = cx.at(0);
        if (ccx == 'E' || ccx == 'e' || ccx == 'W' || ccx == 'w') {
            const auto& cx_num = cx.substr(1);
            if (!ez::isInteger(cx_num)) {
                ret = false;
            }
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

}  // namespace geo
}  // namespace ez
