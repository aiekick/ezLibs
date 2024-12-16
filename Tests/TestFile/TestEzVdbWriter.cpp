#include <ezlibs/ezVdbWriter.hpp>
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

bool TestEzVdbWriter_Writer() {
    const int32_t SIZE = 150;
    const float Z_SCALE = 0.5f;
    const int32_t FRAMES = 10;
    const float len_ratio = 1.0f / (SIZE * SIZE);
    ez::file::vdb::Writer vdb;
    float r, g, b;
    float time = 0.0f;
    for (int32_t f = 0; f < FRAMES; ++f) {
        vdb.setKeyFrame(f);
        auto& floatLayer = vdb.getFloatLayer(0, "density");
        auto& vec3sLayer = vdb.getVec3sLayer(1, "color");
        for (int32_t i = -SIZE; i < SIZE; ++i) {
            for (int32_t j = -SIZE; j < SIZE; ++j) {
                float len = (i * i + j * j) * len_ratio;
                int32_t pz = (int32_t)((std::sin(len * 10.0 + time) * 0.5 + 0.5) * (std::abs(50.0f - 25.0f * len)) * Z_SCALE);
                auto px = i + SIZE;
                auto py = j + SIZE;
                floatLayer.addVoxel(px, py, pz, 1.0f);
                r = sin(len * 10.0f) * 0.5f + 0.5f;
                g = sin(len * 7.0f) * 0.5f + 0.5f;
                b = sin(len * 5.0f) * 0.5f + 0.5f;
                vec3sLayer.addVoxel(px, py, pz, r, g, b);
            }
        }
        time += 0.5f;
    }
    vdb.save(PROJECT_PATH "/test.vdb");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzVdbWriter(const std::string& vTest) {
    IfTestExist(TestEzVdbWriter_Writer);
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
