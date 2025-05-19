#include <ezlibs/wip/ezGif.hpp>
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

bool TestEzGif_Writer() {
    ez::img::Gif gif;
    gif.setSize(100, 100);
    gif.addColor(0, ez::img::Gif::RGB(255, 0, 0));    // Rouge
    gif.addColor(1, ez::img::Gif::RGB(0, 255, 0));    // Vert
    gif.addColor(2, ez::img::Gif::RGB(0, 0, 255));    // Bleu
    gif.addColor(3, ez::img::Gif::RGB(255, 255, 0));  // Jaune
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            uint8_t colorIndex = (x / 25) + (y / 25);  // one color per block of 25x25
            switch (colorIndex) {
                case 0:
                case 6: {
                    gif.addPixel(x, y, 0);
                } break;
                case 1:
                case 5: {
                    gif.addPixel(x, y, 1);
                } break;
                case 2:
                case 4: {
                    gif.addPixel(x, y, 2);
                } break;
                case 3: {
                    gif.addPixel(x, y, 3);
                } break;
                default: break;
            }
        }
    }
    gif.save(RESULTS_PATH "/test.gif");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzGif(const std::string& vTest) {
    IfTestExist(TestEzGif_Writer);
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
