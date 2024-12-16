#include <ezlibs/ezVoxWriter.hpp>
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

bool TestEzVoxWriter_Writer() {
    const int32_t SIZE = 189;
    const int32_t OFFSET = SIZE;
    const float Z_SCALE = 1.0f;
    const int32_t FRAMES = 10;
    const float len_ratio = 1.0f / (SIZE * SIZE);
    ez::file::vox::Writer vox;
    vox.setKeyFrameTimeLoggingFunctor([](const ez::file::vox::KeyFrame& vKeyFrame, const double& vValue) {  //
        std::cout << "Elapsed time for Frame " << vKeyFrame << " : " << vValue << " secs" << std::endl;
    });
    vox.startTimeLogging();
    float time = 0.0f;
    for (int32_t k = 0; k < FRAMES; ++k) {
        vox.setKeyFrame(k);
        for (int32_t i = -SIZE; i < SIZE; ++i) {
            for (int32_t j = -SIZE; j < SIZE; ++j) {
                float len = (i * i + j * j) * len_ratio;
                int32_t pz = (int32_t)((std::sin(len * 10.0 + time) * 0.5 + 0.5) * (std::abs(50.0f - 25.0f * len)) * Z_SCALE);
                int32_t cube_color = (int32_t)(len * 100.0) % 255 + 1;
                vox.addVoxel(i + OFFSET, j + OFFSET, pz, cube_color);
            }
        }
        time += 0.5f;
    }
    vox.stopTimeLogging();
    vox.save(PROJECT_PATH "/test.vox");
    vox.printStats();
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzVoxWriter(const std::string& vTest) {
    IfTestExist(TestEzVoxWriter_Writer);
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
