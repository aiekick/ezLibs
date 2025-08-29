#include <TestEzGeo.h>
#include <ezlibs/ezCTest.hpp>
#include <ezlibs/ezGeo/ezTile.hpp>
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

// ------------------------
// Helpers for this test file
// ------------------------
static inline bool approxEqualDouble(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

// Non-zero min corner to avoid trivial edge cases (lat, lon)
// x = latitude, y = longitude
static const ez::dmsCoord kMinNonZero(0.56, 1.89);

// 2x2 grid (rows=lats, cols=lons) with mixed altitudes: negative and positive
// [ [-50,  0],
//   [ 100, 200] ]
static ez::geo::tile<int16_t>::DatasContainer MakeDatas2x2_AltitudesMixed() {
    return ez::geo::tile<int16_t>::DatasContainer{{static_cast<int16_t>(-50), static_cast<int16_t>(0)}, {static_cast<int16_t>(100), static_cast<int16_t>(200)}};
}

// All-negative altitudes:
// [ [-200, -100],
//   [  -50,  -25] ]
static ez::geo::tile<int16_t>::DatasContainer MakeDatas2x2_AllNeg() {
    return ez::geo::tile<int16_t>::DatasContainer{{static_cast<int16_t>(-200), static_cast<int16_t>(-100)}, {static_cast<int16_t>(-50), static_cast<int16_t>(-25)}};
}

// All-positive altitudes:
// [ [10, 20],
//   [30, 40] ]
static ez::geo::tile<int16_t>::DatasContainer MakeDatas2x2_AllPos() {
    return ez::geo::tile<int16_t>::DatasContainer{{static_cast<int16_t>(10), static_cast<int16_t>(20)}, {static_cast<int16_t>(30), static_cast<int16_t>(40)}};
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// ========== MIXED ALTITUDES TESTS (-/+) ==========
//

bool TestEzTile_Base_NonZeroMin() {
    auto datas = MakeDatas2x2_AltitudesMixed();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);

    // Validity and sizes
    CTEST_ASSERT(tile.isValid());
    CTEST_ASSERT(tile.getNLats() == 2u);
    CTEST_ASSERT(tile.getNLons() == 2u);

    // Min/Max in degrees: max = min + (nLats, nLons) with 1° per step
    const auto& minDms = tile.getMinDms();
    const auto& maxDms = tile.getMaxDms();
    CTEST_ASSERT(approxEqualDouble(minDms.x.toAngle(), 0.56));
    CTEST_ASSERT(approxEqualDouble(minDms.y.toAngle(), 1.89));
    CTEST_ASSERT(approxEqualDouble(maxDms.x.toAngle(), 0.56 + 2.0));
    CTEST_ASSERT(approxEqualDouble(maxDms.y.toAngle(), 1.89 + 2.0));

    return true;
}

bool TestEzTile_Discrete_NonZeroMin() {
    auto datas = MakeDatas2x2_AltitudesMixed();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    // getValue(uvec2) expects {lonIndex, latIndex}; internal access is m_datas[lat][lon]
    int16_t v{};
    CTEST_ASSERT(tile.getValue(ez::uvec2(0u, 0u), v) && v == -50);
    CTEST_ASSERT(tile.getValue(ez::uvec2(1u, 0u), v) && v == 0);
    CTEST_ASSERT(tile.getValue(ez::uvec2(0u, 1u), v) && v == 100);
    CTEST_ASSERT(tile.getValue(ez::uvec2(1u, 1u), v) && v == 200);

    // Out of range must fail (no wrapping for discrete access)
    CTEST_ASSERT(!tile.getValue(ez::uvec2(2u, 0u), v));
    CTEST_ASSERT(!tile.getValue(ez::uvec2(0u, 2u), v));
    CTEST_ASSERT(!tile.getValue(ez::uvec2(3u, 3u), v));

    return true;
}

bool TestEzTile_Continuous_NoWrap_NonZeroMin() {
    auto datas = MakeDatas2x2_AltitudesMixed();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    double v{};
    const double latMin = kMinNonZero.x.toAngle();
    const double lonMin = kMinNonZero.y.toAngle();

    // Exact grid points (should match discrete values)
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 0.0), v) && approxEqualDouble(v, -50.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 1.0, lonMin + 0.0), v) && approxEqualDouble(v, 100.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 1.0), v) && approxEqualDouble(v, 0.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 1.0, lonMin + 1.0), v) && approxEqualDouble(v, 200.0));

    // Center of the 2x2 cell: (lat=+0.5, lon=+0.5) -> 62.5
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 0.5), v) && approxEqualDouble(v, 62.5));

    // Out-of-bounds without wrap -> false
    CTEST_ASSERT(!tile.getValue(ez::dmsCoord(latMin - 0.01, lonMin + 0.0), v));
    CTEST_ASSERT(!tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin - 0.01), v));
    CTEST_ASSERT(!tile.getValue(ez::dmsCoord(latMin + 2.01, lonMin + 0.0), v));
    CTEST_ASSERT(!tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 2.01), v));

    return true;
}

bool TestEzTile_Continuous_Wrap_NonZeroMin() {
    auto datas = MakeDatas2x2_AltitudesMixed();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    double v{};
    const double latMin = kMinNonZero.x.toAngle();
    const double lonMin = kMinNonZero.y.toAngle();

    // Wrap latitude: lat = min + 2.25 -> wraps to min + 0.25 => -12.5
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 0.0), v, /*wrapLat*/ true, /*wrapLon*/ false) && approxEqualDouble(v, -12.5));

    // Wrap longitude: lon = min + 2.5 -> wraps to min + 0.5 => 62.5
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 2.5), v, /*wrapLat*/ false, /*wrapLon*/ true) && approxEqualDouble(v, 62.5));

    // Wrap both axes: (lat=+2.25->+0.25, lon=+2.5->+0.5) -> 18.75
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 2.5), v, /*wrapLat*/ true, /*wrapLon*/ true) && approxEqualDouble(v, 18.75));

    // Without wrap, same queries should fail (out-of-bounds)
    CTEST_ASSERT(!tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 0.0), v, /*wrapLat*/ false, /*wrapLon*/ false));
    CTEST_ASSERT(!tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 2.5), v, /*wrapLat*/ false, /*wrapLon*/ false));

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// ========== ALL-NEGATIVE TABLE TESTS ==========
//

bool TestEzTile_AllNeg_Discrete() {
    auto datas = MakeDatas2x2_AllNeg();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    int16_t v{};
    // getValue(uvec2) expects {lonIndex, latIndex}
    CTEST_ASSERT(tile.getValue(ez::uvec2(0u, 0u), v) && v == -200);
    CTEST_ASSERT(tile.getValue(ez::uvec2(1u, 0u), v) && v == -100);
    CTEST_ASSERT(tile.getValue(ez::uvec2(0u, 1u), v) && v == -50);
    CTEST_ASSERT(tile.getValue(ez::uvec2(1u, 1u), v) && v == -25);

    // Out of range must fail (no wrapping)
    CTEST_ASSERT(!tile.getValue(ez::uvec2(2u, 0u), v));
    CTEST_ASSERT(!tile.getValue(ez::uvec2(0u, 2u), v));
    return true;
}

bool TestEzTile_AllNeg_Continuous_NoWrap() {
    auto datas = MakeDatas2x2_AllNeg();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    double v{};
    const double latMin = kMinNonZero.x.toAngle();
    const double lonMin = kMinNonZero.y.toAngle();

    // Exact grid points
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 0.0), v) && approxEqualDouble(v, -200.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 1.0), v) && approxEqualDouble(v, -100.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 1.0, lonMin + 0.0), v) && approxEqualDouble(v, -50.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 1.0, lonMin + 1.0), v) && approxEqualDouble(v, -25.0));

    // Center interpolation @ (lat +0.5, lon +0.5) -> -93.75
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 0.5), v) && approxEqualDouble(v, -93.75));

    return true;
}

bool TestEzTile_AllNeg_Continuous_Wrap() {
    auto datas = MakeDatas2x2_AllNeg();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    double v{};
    const double latMin = kMinNonZero.x.toAngle();
    const double lonMin = kMinNonZero.y.toAngle();

    // Wrap latitude: lat = min + 2.25 -> wraps to min + 0.25 -> -162.5
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 0.0), v, /*wrapLat*/ true, /*wrapLon*/ false) && approxEqualDouble(v, -162.5));

    // Wrap longitude: lon = min + 2.5 -> wraps to min + 0.5 -> -93.75
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 2.5), v, /*wrapLat*/ false, /*wrapLon*/ true) && approxEqualDouble(v, -93.75));

    // Wrap both axes: -> -121.875
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 2.5), v, /*wrapLat*/ true, /*wrapLon*/ true) && approxEqualDouble(v, -121.875));

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// ========== ALL-POSITIVE TABLE TESTS ==========
//

bool TestEzTile_AllPos_Discrete() {
    auto datas = MakeDatas2x2_AllPos();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    int16_t v{};
    CTEST_ASSERT(tile.getValue(ez::uvec2(0u, 0u), v) && v == 10);
    CTEST_ASSERT(tile.getValue(ez::uvec2(1u, 0u), v) && v == 20);
    CTEST_ASSERT(tile.getValue(ez::uvec2(0u, 1u), v) && v == 30);
    CTEST_ASSERT(tile.getValue(ez::uvec2(1u, 1u), v) && v == 40);

    // Out-of-range without wrap
    CTEST_ASSERT(!tile.getValue(ez::uvec2(2u, 0u), v));
    CTEST_ASSERT(!tile.getValue(ez::uvec2(0u, 2u), v));
    return true;
}

bool TestEzTile_AllPos_Continuous_NoWrap() {
    auto datas = MakeDatas2x2_AllPos();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    double v{};
    const double latMin = kMinNonZero.x.toAngle();
    const double lonMin = kMinNonZero.y.toAngle();

    // Exact grid points
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 0.0), v) && approxEqualDouble(v, 10.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.0, lonMin + 1.0), v) && approxEqualDouble(v, 20.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 1.0, lonMin + 0.0), v) && approxEqualDouble(v, 30.0));
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 1.0, lonMin + 1.0), v) && approxEqualDouble(v, 40.0));

    // Center interpolation @ (lat +0.5, lon +0.5) -> 25
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 0.5), v) && approxEqualDouble(v, 25.0));

    return true;
}

bool TestEzTile_AllPos_Continuous_Wrap() {
    auto datas = MakeDatas2x2_AllPos();
    ez::geo::tile<int16_t> tile(datas, kMinNonZero);
    CTEST_ASSERT(tile.isValid());

    double v{};
    const double latMin = kMinNonZero.x.toAngle();
    const double lonMin = kMinNonZero.y.toAngle();

    // Wrap latitude: lat = min + 2.25 -> wraps to min + 0.25 => 15
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 0.0), v, /*wrapLat*/ true, /*wrapLon*/ false) && approxEqualDouble(v, 15.0));

    // Wrap longitude: lon = min + 2.5 -> wraps to min + 0.5 => 25
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 0.5, lonMin + 2.5), v, /*wrapLat*/ false, /*wrapLon*/ true) && approxEqualDouble(v, 25.0));

    // Wrap both: (lat +2.25 -> +0.25, lon +2.5 -> +0.5) => 20
    CTEST_ASSERT(tile.getValue(ez::dmsCoord(latMin + 2.25, lonMin + 2.5), v, /*wrapLat*/ true, /*wrapLon*/ true) && approxEqualDouble(v, 20.0));

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzTile(const std::string& vTest) {
    // Mixed altitudes tests
    IfTestExist(TestEzTile_Base_NonZeroMin);
    IfTestExist(TestEzTile_Discrete_NonZeroMin);
    IfTestExist(TestEzTile_Continuous_NoWrap_NonZeroMin);
    IfTestExist(TestEzTile_Continuous_Wrap_NonZeroMin);

    // All-negative table tests
    IfTestExist(TestEzTile_AllNeg_Discrete);
    IfTestExist(TestEzTile_AllNeg_Continuous_NoWrap);
    IfTestExist(TestEzTile_AllNeg_Continuous_Wrap);

    // All-positive table tests
    IfTestExist(TestEzTile_AllPos_Discrete);
    IfTestExist(TestEzTile_AllPos_Continuous_NoWrap);
    IfTestExist(TestEzTile_AllPos_Continuous_Wrap);

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
