#include <ezlibs/ezMath.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzScreen_FloorNb_Scalar_Float() {
    float v = 3.9f;
    auto r = ez::screen::floor_nb<float>(v);
    CTEST_ASSERT(r == 3.0f);
    return true;
}

bool TestEzScreen_FloorNb_Scalar_Double() {
    double v = -2.1;
    auto r = ez::screen::floor_nb<double>(v);
    CTEST_ASSERT(r == -3.0);
    return true;
}

bool TestEzScreen_FloorNb_Scalar_Int_NoOp() {
    int v = 7;
    auto r = ez::screen::floor_nb<int>(v);
    CTEST_ASSERT(r == 7);
    return true;
}

bool TestEzScreen_FloorNb_Vec4_Float() {
    ez::vec4<float> v{3.9f, -2.1f, 5.0f, 0.0001f};
    auto r = ez::screen::floor_nb<float>(v);
    CTEST_ASSERT(r.x == 3.0f);
    CTEST_ASSERT(r.y == -3.0f);
    CTEST_ASSERT(r.z == 5.0f);
    CTEST_ASSERT(r.w == 0.0f);
    return true;
}

bool TestEzScreen_FloorNb_Vec4_Int_NoOp() {
    ez::vec4<int> v{3, -2, 5, 0};
    auto r = ez::screen::floor_nb<int>(v);
    CTEST_ASSERT(r.x == 3);
    CTEST_ASSERT(r.y == -2);
    CTEST_ASSERT(r.z == 5);
    CTEST_ASSERT(r.w == 0);
    return true;
}

bool TestEzScreen_GetScreenRectWithSize_Pillarbox() {
    // item 50x100 dans viewport 300x300 -> newX=150 < 300 => bandes verticales (pillarbox)
    ez::fvec2 item{50.f, 100.f};
    ez::fvec2 maxs{300.f, 300.f};
    auto rc = ez::screen::getScreenRectWithSize<float>(item, maxs);
    CTEST_ASSERT(rc.x == 75.0f);  // (300-150)/2
    CTEST_ASSERT(rc.y == 0.0f);
    CTEST_ASSERT(rc.z == 150.0f);
    CTEST_ASSERT(rc.w == 300.0f);
    return true;
}

bool TestEzScreen_GetScreenRectWithSize_Letterbox() {
    // item 100x50 dans viewport 300x300 -> newX=600 > 300 => bandes horizontales (letterbox)
    ez::fvec2 item{100.f, 50.f};
    ez::fvec2 maxs{300.f, 300.f};
    auto rc = ez::screen::getScreenRectWithSize<float>(item, maxs);
    CTEST_ASSERT(rc.x == 0.0f);
    CTEST_ASSERT(rc.y == 75.0f);  // (300-150)/2
    CTEST_ASSERT(rc.z == 300.0f);
    CTEST_ASSERT(rc.w == 150.0f);
    return true;
}

bool TestEzScreen_GetScreenRectWithRatio_2dot0_Letterbox() {
    // ratio 2.0 dans viewport 300x300 -> identique au cas item 100x50
    ez::fvec2 maxs{300.f, 300.f};
    auto rc = ez::screen::getScreenRectWithRatio<float>(2.0f, maxs);
    CTEST_ASSERT(rc.x == 0.0f);
    CTEST_ASSERT(rc.y == 75.0f);
    CTEST_ASSERT(rc.z == 300.0f);
    CTEST_ASSERT(rc.w == 150.0f);
    return true;
}

bool TestEzScreen_GetScreenRectWithRatio_0dot5_Pillarbox() {
    // ratio 0.5 dans viewport 300x300 -> identique au cas item 50x100
    ez::fvec2 maxs{300.f, 300.f};
    auto rc = ez::screen::getScreenRectWithRatio<float>(0.5f, maxs);
    CTEST_ASSERT(rc.x == 75.0f);
    CTEST_ASSERT(rc.y == 0.0f);
    CTEST_ASSERT(rc.z == 150.0f);
    CTEST_ASSERT(rc.w == 300.0f);
    return true;
}

bool TestEzScreen_ScreenToWorld_Centered_YDown_And_YUp() {
    const ez::fvec2 viewport{200.f, 100.f};
    const ez::fvec2 mouseCenter{100.f, 50.f};
    ez::fvec2 worldCenter{0.f, 0.f};
    const float sPx = 10.0f;

    // À centre -> monde == centre
    auto w1 = ez::screen::screenToWorld_centered(mouseCenter, viewport, worldCenter, sPx, true);
    CTEST_ASSERT(std::abs(w1.x - 0.0f) < 1e-6f);
    CTEST_ASSERT(std::abs(w1.y - 0.0f) < 1e-6f);

    auto w2 = ez::screen::screenToWorld_centered(mouseCenter, viewport, worldCenter, sPx, false);
    CTEST_ASSERT(std::abs(w2.x - 0.0f) < 1e-6f);
    CTEST_ASSERT(std::abs(w2.y - 0.0f) < 1e-6f);

    // Point décalé (150,50) -> dx=50, dy=0
    ez::fvec2 mouseRight{150.f, 50.f};
    auto w3 = ez::screen::screenToWorld_centered(mouseRight, viewport, worldCenter, sPx, true);
    CTEST_ASSERT(std::abs(w3.x - 5.0f) < 1e-6f);
    CTEST_ASSERT(std::abs(w3.y - 0.0f) < 1e-6f);

    return true;
}

bool TestEzScreen_CenteredZoom_KeepPointUnderMouse() {
    const ez::fvec2 viewport{200.f, 100.f};
    const ez::fvec2 mouse{150.f, 50.f};  // dx=50, dy=0
    const bool yDown = true;
    const float sFitPx = 10.0f;

    float zoom = 1.0f;
    ez::fvec2 center{0.f, 0.f};

    // avant : sPx = 10, worldUnderMouse = (5,0)
    ez::screen::centeredZoom(2.0f, mouse, viewport, yDown, sFitPx, zoom, center);

    CTEST_ASSERT(std::abs(zoom - 2.0f) < 1e-6f);
    // après : sPx=20, nouveau centre = (5 - 50/20, 0) = (2.5, 0)
    CTEST_ASSERT(std::abs(center.x - 2.5f) < 1e-6f);
    CTEST_ASSERT(std::abs(center.y - 0.0f) < 1e-6f);
    return true;
}

bool TestEzScreen_ZoomedTranslation_YDown() {
    const bool yDown = true;
    const float sFitPx = 10.0f;
    const float zoom = 2.0f;  // sPx = 20
    ez::fvec2 center{0.f, 0.f};

    ez::fvec2 drag{20.f, -10.f};  // dx=20, dy=-10 (yDown)
    ez::screen::zoomedTranslation(drag, yDown, sFitPx, zoom, center);

    // centre -= (dx/sPx, dy/sPx) => (-1.0, +0.5)
    CTEST_ASSERT(std::abs(center.x + 1.0f) < 1e-6f);
    CTEST_ASSERT(std::abs(center.y - 0.5f) < 1e-6f);
    return true;
}

bool TestEzScreen_ComputeFitScalePx() {
    ez::fvec2 worldMin{0.f, 0.f};
    ez::fvec2 worldMax{100.f, 50.f};
    ez::fvec2 viewport{300.f, 300.f};
    float sPx = ez::screen::computeFitScalePx(worldMin, worldMax, viewport);
    // attendu : 3 (car rc.z = 300 pour W=100)
    CTEST_ASSERT(std::abs(sPx - 3.0f) < 1e-6f);
    return true;
}

bool TestEzScreen_ComputeFitToContent() {
    ez::fvec2 worldMin{0.f, 0.f};
    ez::fvec2 worldMax{100.f, 50.f};
    ez::fvec2 framebuffer{300.f, 300.f};

    ez::fvec2 origin{};
    float scale = 0.f, inv = 0.f;

    ez::screen::computeFitToContent(worldMin, worldMax, framebuffer, origin, scale, inv);

    // attendu : scale = 3, inv = 1/3, origin = (0, 75)
    CTEST_ASSERT(std::abs(scale - 3.0f) < 1e-6f);
    CTEST_ASSERT(std::abs(inv - (1.0f / 3.0f)) < 1e-6f);
    CTEST_ASSERT(std::abs(origin.x - 0.0f) < 1e-6f);
    CTEST_ASSERT(std::abs(origin.y - 75.0f) < 1e-6f);
    return true;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzScreen(const std::string& vTest) {
    IfTestExist(TestEzScreen_FloorNb_Scalar_Float);
    IfTestExist(TestEzScreen_FloorNb_Scalar_Double);
    IfTestExist(TestEzScreen_FloorNb_Scalar_Int_NoOp);
    IfTestExist(TestEzScreen_FloorNb_Vec4_Float);
    IfTestExist(TestEzScreen_FloorNb_Vec4_Int_NoOp);
    IfTestExist(TestEzScreen_GetScreenRectWithSize_Pillarbox);
    IfTestExist(TestEzScreen_GetScreenRectWithSize_Letterbox);
    IfTestExist(TestEzScreen_GetScreenRectWithRatio_2dot0_Letterbox);
    IfTestExist(TestEzScreen_GetScreenRectWithRatio_0dot5_Pillarbox);
    IfTestExist(TestEzScreen_ScreenToWorld_Centered_YDown_And_YUp);
    IfTestExist(TestEzScreen_CenteredZoom_KeepPointUnderMouse);
    IfTestExist(TestEzScreen_ZoomedTranslation_YDown);
    IfTestExist(TestEzScreen_ComputeFitScalePx);
    IfTestExist(TestEzScreen_ComputeFitToContent);
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

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
