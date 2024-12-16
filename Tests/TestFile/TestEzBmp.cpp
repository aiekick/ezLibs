#include <ezlibs/ezBmp.hpp>
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

bool TestEzBmp_Writer() {
    ez::img::Bmp bmp;
    bmp.setSize(100, 100);
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            uint8_t colorIndex = (x / 25) + (y / 25);  // one color per block of 25x25
            switch (colorIndex) {
                case 0:
                case 6: {
                    bmp.setPixel(x, y, 255, 0, 0);
                } break;
                case 1:
                case 5: {
                    bmp.setPixel(x, y, 0, 255, 0);
                } break;
                case 2:
                case 4: {
                    bmp.setPixel(x, y, 0, 0, 255);
                } break;
                case 3: {
                    bmp.setPixel(x, y, 255, 255, 0);
                } break;
                default: break;
            }
        }
    }
    bmp.save(PROJECT_PATH "/test.bmp");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzBmp(const std::string& vTest) {
    IfTestExist(TestEzBmp_Writer);
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
