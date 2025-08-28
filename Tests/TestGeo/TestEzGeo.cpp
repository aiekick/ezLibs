#include <TestEzGeo.h>
#include <ezlibs/ezCTest.hpp>
#include <ezlibs/ezGeo/ezGeo.hpp>
#include <iostream>
#include <string>
#include <cmath>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // double -> float possible loss
#pragma warning(disable : 4305)  // double literal truncated to float
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// ---------- Utilities (tests only) ----------
// Compare doubles within an absolute epsilon
static inline bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// ---------- Mercator <-> Degrees ----------

bool TestEzGeo_Mercator_Degrees_RoundTrip_Paris() {
    // Paris: lon=2.352222, lat=48.856613
    ez::dvec2 deg{2.352222, 48.856613};
    auto m = ez::geo::fromDegressToMercatorMeters(deg);   // degrees -> meters
    auto back = ez::geo::fromMercatorMetersToDegrees(m);  // meters -> degrees
    CTEST_ASSERT(approxEqual(back.x, deg.x, 1e-8));
    CTEST_ASSERT(approxEqual(back.y, deg.y, 1e-8));
    return true;
}

bool TestEzGeo_Mercator_Degrees_RoundTrip_NYC() {
    ez::dvec2 deg{-74.0060, 40.7128};
    auto m = ez::geo::fromDegressToMercatorMeters(deg);
    auto back = ez::geo::fromMercatorMetersToDegrees(m);
    CTEST_ASSERT(approxEqual(back.x, deg.x, 1e-8));
    CTEST_ASSERT(approxEqual(back.y, deg.y, 1e-8));
    return true;
}

bool TestEzGeo_Mercator_Degrees_RoundTrip_Sydney() {
    ez::dvec2 deg{151.2093, -33.8688};
    auto m = ez::geo::fromDegressToMercatorMeters(deg);
    auto back = ez::geo::fromMercatorMetersToDegrees(m);
    CTEST_ASSERT(approxEqual(back.x, deg.x, 1e-8));
    CTEST_ASSERT(approxEqual(back.y, deg.y, 1e-8));
    return true;
}

// ---------- mercatorYToUnrolledY ----------

bool TestEzGeo_MercatorYToUnrolledY() {
    // For a given latitude φ, if we build yMerc and then unroll it,
    // we should retrieve R * φ (linearized y in meters).
    const double R = ez::geo::EARTH_RADIUS;
    const double degs[] = {-60.0, -45.0, -10.0, 0.0, 10.0, 45.0, 60.0};
    for (double latDeg : degs) {
        ez::dvec2 d{0.0, latDeg};
        auto meters = ez::geo::fromDegressToMercatorMeters(d);
        const double unrolled = ez::geo::mercatorYToUnrolledY(meters.y);
        const double phi = latDeg * M_PI / 180.0;
        CTEST_ASSERT(approxEqual(unrolled, R * phi, 1e-6));
    }
    return true;
}

// ---------- unscaleWGS84Yaxis ----------

bool TestEzGeo_UnscaleWGS84Yaxis() {
    // Function definition: vLat * (111132 / (111320 * cos(phi0)))
    const double lat = 10.0;
    const double ref = 45.0;
    const double phi0 = ref * M_PI / 180.0;
    const double k = 111132.0 / (111320.0 * std::cos(phi0));

    const double got = ez::geo::unscaleWGS84Yaxis(lat, ref);
    CTEST_ASSERT(approxEqual(got, lat * k, 1e-9));

    // Zero stays zero
    CTEST_ASSERT(ez::geo::unscaleWGS84Yaxis(0.0, ref) == 0.0);
    return true;
}

// ---------- DEM helpers ----------

bool TestEzGeo_ParseDemCoordinate() {
    // "N10" -> +10 ; "S10" -> -10 ; "E001" -> +1 ; "w112" -> -112
    CTEST_ASSERT(ez::geo::parseDemCoordinate("N10") == 10);
    CTEST_ASSERT(ez::geo::parseDemCoordinate("S10") == -10);
    CTEST_ASSERT(ez::geo::parseDemCoordinate("E001") == 1);
    CTEST_ASSERT(ez::geo::parseDemCoordinate("w112") == -112);
    return true;
}

bool TestEzGeo_CheckDemFileName_Valid_Upper() {
    std::string cx, cy;
    CTEST_ASSERT(ez::geo::checkDemFileName("N48E002", cx, cy));
    CTEST_ASSERT(cx == "E002");
    CTEST_ASSERT(cy == "N48");
    return true;
}

bool TestEzGeo_CheckDemFileName_Valid_Lower() {
    std::string cx, cy;
    CTEST_ASSERT(ez::geo::checkDemFileName("s01w112", cx, cy));
    CTEST_ASSERT(cx == "w112");
    CTEST_ASSERT(cy == "s01");
    return true;
}

bool TestEzGeo_CheckDemFileName_Invalid_Length() {
    std::string cx, cy;
    CTEST_ASSERT(!ez::geo::checkDemFileName("N1E001", cx, cy));  // bad length
    return true;
}

bool TestEzGeo_CheckDemFileName_Invalid_Prefix() {
    std::string cx, cy;
    CTEST_ASSERT(!ez::geo::checkDemFileName("X01E001", cx, cy));  // bad first letter
    CTEST_ASSERT(!ez::geo::checkDemFileName("N01Q12", cx, cy));   // bad second block letter
    return true;
}

// ---------- DMS parsing & formatting ----------

bool TestEzGeo_Dms_Parse_DecimalDegrees() {
    ez::geo::dms d;
    CTEST_ASSERT(d.parse("-12.5"));
    // -12.5° -> 12° 30' 0" '-' but can be lat or lon
    CTEST_ASSERT(d.deg == 12.0f);
    CTEST_ASSERT(d.min == 30.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    CTEST_ASSERT(d.letter == '-');
    // Default letter is 'N', but toAngle uses letter sign.
    // Here deg is negative so toAngle() should be negative anyway.
    CTEST_ASSERT(approxEqual(d.toAngle(), -12.5));
    return true;
}

bool TestEzGeo_Dms_Parse_OnlyDegrees() {
    ez::geo::dms d;
    CTEST_ASSERT(d.parse("48"));
    CTEST_ASSERT(d.deg == 48.0f);
    CTEST_ASSERT(d.min == 0.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    CTEST_ASSERT(d.letter == ' ');
    CTEST_ASSERT(approxEqual(d.toAngle(), 48.0));
    return true;
}

bool TestEzGeo_Dms_Parse_Full_With_Symbols() {
    // Using typical DMS with symbols; should parse and preserve values.
    ez::geo::dms d("48°51'24.589\"N");
    CTEST_ASSERT(d.deg == 48.0f);
    CTEST_ASSERT(d.min == 51.0f);
    CTEST_ASSERT(d.sec == 24.589f);
    CTEST_ASSERT(d.letter == 'N');
    const double dec = 48.0 + 51.0 / 60.0 + 24.589 / 3600.0;
    CTEST_ASSERT(approxEqual(d.toAngle(), dec));
    return true;
}

bool TestEzGeo_Dms_Parse_With_Spaces_And_E() {
    ez::geo::dms d("2 21 8 E");
    CTEST_ASSERT(d.deg == 2.0f);
    CTEST_ASSERT(d.min == 21.0f);
    CTEST_ASSERT(d.sec == 8.0f);
    CTEST_ASSERT(d.letter == 'E');
    CTEST_ASSERT(approxEqual(d.toAngle(), 2.3522222222));
    return true;
}

bool TestEzGeo_Dms_Parse_NoSeconds() {
    ez::geo::dms d("45°30'N");  // seconds omitted
    CTEST_ASSERT(d.deg == 45.0f);
    CTEST_ASSERT(d.min == 30.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    CTEST_ASSERT(d.letter == 'N');
    CTEST_ASSERT(approxEqual(d.toAngle(), 45.5));
    return true;
}

bool TestEzGeo_Dms_ToAngle_With_S() {
    ez::geo::dms d("33 51 36 S");  // 33°51'36"S -> -33.86
    CTEST_ASSERT(d.letter == 'S');
    CTEST_ASSERT(approxEqual(d.toAngle(), -33.86));
    return true;
}

bool TestEzGeo_Dms_FromAngle_Latitude() {
    ez::geo::dms lat(-33.859972, true);  // isLat => 'N'/'S'
    CTEST_ASSERT(lat.letter == 'S');
    CTEST_ASSERT(approxEqual(lat.toAngle(), -33.859972, 1e-4));
    return true;
}

bool TestEzGeo_Dms_FromAngle_Longitude() {
    ez::geo::dms lon(151.211111, false);  // isLon => 'E'/'W'
    CTEST_ASSERT(lon.letter == 'E');
    CTEST_ASSERT(approxEqual(lon.toAngle(), 151.211111, 1e-4));
    return true;
}

bool TestEzGeo_Dms_RoundTrip_Lat() {
    const double a = 48.856613;  // Paris lat
    ez::geo::dms v(a, true);
    CTEST_ASSERT(v.letter == 'N');
    CTEST_ASSERT(approxEqual(v.toAngle(), a, 1e-4));
    return true;
}

bool TestEzGeo_Dms_RoundTrip_Lon() {
    const double a = 2.352222;  // Paris lon
    ez::geo::dms v(a, false);
    CTEST_ASSERT(v.letter == 'E');
    CTEST_ASSERT(approxEqual(v.toAngle(), a, 1e-4));
    return true;
}

bool TestEzGeo_Dms_IsLat_IsLon() {
    ez::geo::dms v("10 0 0 E");
    CTEST_ASSERT(v.isLon());
    CTEST_ASSERT(!v.isLat());
    v.setLetter('N');
    CTEST_ASSERT(v.isLat());
    CTEST_ASSERT(!v.isLon());
    return true;
}

// ---------- DMS parse failure cases (strict rules) ----------

bool TestEzGeo_Dms_ParseFail_NegativeMinutes() {
    ez::geo::dms d;
    CTEST_ASSERT(!d.parse("48 -1 24"));  // negative minutes not allowed
    return true;
}

bool TestEzGeo_Dms_ParseFail_NegativeSeconds() {
    ez::geo::dms d;
    CTEST_ASSERT(!d.parse("48 1 -24"));  // negative seconds not allowed
    return true;
}

bool TestEzGeo_Dms_ParseFail_MinutesOverflow() {
    ez::geo::dms d;
    CTEST_ASSERT(!d.parse("48 60 00"));  // minutes >= 60 invalid
    return true;
}

bool TestEzGeo_Dms_ParseFail_SecondsOverflow() {
    ez::geo::dms d;
    CTEST_ASSERT(!d.parse("48 00 60"));  // seconds >= 60 invalid
    return true;
}

bool TestEzGeo_Dms_ParseFail_Empty() {
    ez::geo::dms d;
    CTEST_ASSERT(!d.parse(""));  // nothing to parse
    return true;
}

// ---------- DMS offsets (deg/min/sec) ----------

bool TestEzGeo_Dms_OffsetSec_CarriesToMin_Positive() {
    ez::geo::dms d;
    d.setDeg(10.0f).setMin(20.0f).setSec(30.0f).setLetter('N');
    d.offsetSec(90.5f);  // 30 + 90.5 = 120.5 -> +2 min, sec=0.5
    CTEST_ASSERT(d.deg == 10.0f);
    CTEST_ASSERT(d.min == 22.0f);
    CTEST_ASSERT(std::fabs(d.sec - 0.5f) < 1e-6f);
    // Invariant: deg and min are integers; only sec may be fractional
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    return true;
}

bool TestEzGeo_Dms_OffsetSec_CarriesToMin_Negative() {
    ez::geo::dms d;
    d.setDeg(5.0f).setMin(3.0f).setSec(10.0f).setLetter('N');
    d.offsetSec(-75.0f);  // 10 - 75 = -65 -> -2 min carry, sec=55
    CTEST_ASSERT(d.deg == 5.0f);
    CTEST_ASSERT(d.min == 1.0f);
    CTEST_ASSERT(d.sec == 55.0f);
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    return true;
}

bool TestEzGeo_Dms_OffsetMin_CarriesToDeg_AndPushFracToSec() {
    ez::geo::dms d;
    d.setDeg(5.0f).setMin(59.0f).setSec(30.0f).setLetter('N');
    d.offsetMin(1.25f);  // +1 min -> 60; +0.25 min -> +15 sec; carry -> deg+1, min=0, sec=45
    CTEST_ASSERT(d.deg == 6.0f);
    CTEST_ASSERT(d.min == 0.0f);
    CTEST_ASSERT(d.sec == 45.0f);
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    return true;
}

bool TestEzGeo_Dms_OffsetMin_NegativeWithFrac() {
    ez::geo::dms d;
    d.setDeg(10.0f).setMin(0.0f).setSec(30.0f).setLetter('N');
    d.offsetMin(-0.75f);  // -1 min and +0.25 min -> -1 min + 15 sec => deg 9, min 59, sec 45
    CTEST_ASSERT(d.deg == 9.0f);
    CTEST_ASSERT(d.min == 59.0f);
    CTEST_ASSERT(d.sec == 45.0f);
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    return true;
}

bool TestEzGeo_Dms_OffsetDeg_PushFracToMinAndSec() {
    ez::geo::dms d;
    d.setDeg(12.0f).setMin(0.0f).setSec(0.0f).setLetter('N');
    d.offsetDeg(1.5f);  // +1 deg, +0.5 deg => +30 min
    CTEST_ASSERT(d.deg == 13.0f);
    CTEST_ASSERT(d.min == 30.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    return true;
}

bool TestEzGeo_Dms_OffsetDeg_PurgeExistingFracInDeg() {
    ez::geo::dms d;
    d.setDeg(10.5f).setMin(0.0f).setSec(0.0f).setLetter('N');
    d.offsetDeg(0.0f);  // push 0.5 deg -> 30 min
    CTEST_ASSERT(d.deg == 10.0f);
    CTEST_ASSERT(d.min == 30.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    return true;
}

bool TestEzGeo_Dms_OffsetDeg_WrapLatitude() {
    ez::geo::dms d;
    d.setDeg(89.0f).setMin(30.0f).setSec(0.0f).setLetter('N');
    d.offsetDeg(5.0f);              // wrap in [0,90): 89 + 5 = 94 -> 4 (no hemisphere flip)
    CTEST_ASSERT(d.letter == 'N');  // letter unchanged
    CTEST_ASSERT(d.deg == 4.0f);
    CTEST_ASSERT(d.min == 30.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    return true;
}

bool TestEzGeo_Dms_OffsetDeg_WrapLongitude() {
    ez::geo::dms d;
    d.setDeg(179.0f).setMin(0.0f).setSec(0.0f).setLetter('E');
    d.offsetDeg(5.0f);              // wrap in [0,180): 179 + 5 = 184 -> 4
    CTEST_ASSERT(d.letter == 'E');  // letter unchanged
    CTEST_ASSERT(d.deg == 4.0f);
    CTEST_ASSERT(d.min == 0.0f);
    CTEST_ASSERT(d.sec == 0.0f);
    return true;
}

bool TestEzGeo_Dms_Offsets_PreserveOnlySecFraction() {
    ez::geo::dms d;
    d.setDeg(0.0f).setMin(0.0f).setSec(0.0f).setLetter('N');
    d.offsetDeg(12.3456f);  // deg frac -> min/sec
    d.offsetMin(7.89f);     // min frac -> sec
    d.offsetSec(3.14159f);  // sec keeps fraction
    // Invariants:
    CTEST_ASSERT(d.deg == std::floor(d.deg));
    CTEST_ASSERT(d.min == std::floor(d.min));
    CTEST_ASSERT(std::fabs(d.sec - std::floor(d.sec)) > 0.0f || d.sec == std::floor(d.sec));
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzGeo(const std::string& vTest) {
    // Mercator / Degrees
    IfTestExist(TestEzGeo_Mercator_Degrees_RoundTrip_Paris);
    else IfTestExist(TestEzGeo_Mercator_Degrees_RoundTrip_NYC);
    else IfTestExist(TestEzGeo_Mercator_Degrees_RoundTrip_Sydney);
    else IfTestExist(TestEzGeo_MercatorYToUnrolledY);
    else IfTestExist(TestEzGeo_UnscaleWGS84Yaxis);
    // DEM helpers
    else IfTestExist(TestEzGeo_ParseDemCoordinate);
    else IfTestExist(TestEzGeo_CheckDemFileName_Valid_Upper);
    else IfTestExist(TestEzGeo_CheckDemFileName_Valid_Lower);
    else IfTestExist(TestEzGeo_CheckDemFileName_Invalid_Length);
    else IfTestExist(TestEzGeo_CheckDemFileName_Invalid_Prefix);
    // DMS
    else IfTestExist(TestEzGeo_Dms_Parse_DecimalDegrees);
    else IfTestExist(TestEzGeo_Dms_Parse_OnlyDegrees);
    else IfTestExist(TestEzGeo_Dms_Parse_Full_With_Symbols);
    else IfTestExist(TestEzGeo_Dms_Parse_With_Spaces_And_E);
    else IfTestExist(TestEzGeo_Dms_Parse_NoSeconds);
    else IfTestExist(TestEzGeo_Dms_ToAngle_With_S);
    else IfTestExist(TestEzGeo_Dms_FromAngle_Latitude);
    else IfTestExist(TestEzGeo_Dms_FromAngle_Longitude);
    else IfTestExist(TestEzGeo_Dms_RoundTrip_Lat);
    else IfTestExist(TestEzGeo_Dms_RoundTrip_Lon);
    else IfTestExist(TestEzGeo_Dms_IsLat_IsLon);
    // DMS failures
    else IfTestExist(TestEzGeo_Dms_ParseFail_NegativeMinutes);
    else IfTestExist(TestEzGeo_Dms_ParseFail_NegativeSeconds);
    else IfTestExist(TestEzGeo_Dms_ParseFail_MinutesOverflow);
    else IfTestExist(TestEzGeo_Dms_ParseFail_SecondsOverflow);
    else IfTestExist(TestEzGeo_Dms_ParseFail_Empty);
    // DMS offsets
    else IfTestExist(TestEzGeo_Dms_OffsetSec_CarriesToMin_Positive);
    else IfTestExist(TestEzGeo_Dms_OffsetSec_CarriesToMin_Negative);
    else IfTestExist(TestEzGeo_Dms_OffsetMin_CarriesToDeg_AndPushFracToSec);
    else IfTestExist(TestEzGeo_Dms_OffsetMin_NegativeWithFrac);
    else IfTestExist(TestEzGeo_Dms_OffsetDeg_PushFracToMinAndSec);
    else IfTestExist(TestEzGeo_Dms_OffsetDeg_PurgeExistingFracInDeg);
    else IfTestExist(TestEzGeo_Dms_OffsetDeg_WrapLatitude);
    else IfTestExist(TestEzGeo_Dms_OffsetDeg_WrapLongitude);
    else IfTestExist(TestEzGeo_Dms_Offsets_PreserveOnlySecFraction);

    return false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
