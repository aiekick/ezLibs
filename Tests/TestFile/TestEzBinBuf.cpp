#include <ezlibs/ezBinBuf.hpp>
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
bool TestEzBinBuf_WriteReadValueLE() {
    ez::BinBuf buf;
    int32_t val = 0x12345678;
    buf.writeValueLE(val);
    size_t pos = 0;
    int32_t readVal = buf.readValueLE<int32_t>(pos);
    CTEST_ASSERT(readVal == val);
    return true;
}

bool TestEzBinBuf_WriteReadValueBE() {
    ez::BinBuf buf;
    int32_t val = 0x12345678;
    buf.writeValueBE(val);
    size_t pos = 0;
    int32_t readVal = buf.readValueBE<int32_t>(pos);
    CTEST_ASSERT(readVal == val);
    return true;
}

bool TestEzBinBuf_WriteReadArrayLE() {
    ez::BinBuf buf;
    uint16_t arr[3] = {1, 2, 3};
    buf.writeArrayLE(arr, 3);
    size_t pos = 0;
    uint16_t out[3] = {};
    buf.readArrayLE(pos, out, 3);
    for (int i = 0; i < 3; ++i) {
        CTEST_ASSERT(out[i] == arr[i]);
    }
    return true;
}

bool TestEzBinBuf_WriteReadArrayBE() {
    ez::BinBuf buf;
    float arr[2] = {3.14f, 2.71f};
    buf.writeArrayBE(arr, 2);
    size_t pos = 0;
    float out[2] = {};
    buf.readArrayBE(pos, out, 2);
    for (int i = 0; i < 2; ++i) {
        CTEST_ASSERT(out[i] == arr[i]);
    }
    return true;
}

bool TestEzBinBuf_SetDatasGetDatas() {
    ez::BinBuf buf;
    std::vector<uint8_t> data = {10, 20, 30};
    buf.setDatas(data);
    const auto& result = buf.getDatas();
    CTEST_ASSERT(result == data);
    return true;
}

bool TestEzBinBuf_ClearSize() {
    ez::BinBuf buf;
    buf.writeValueLE<int32_t>(123);
    CTEST_ASSERT(buf.size() == sizeof(int32_t));
    buf.clear();
    CTEST_ASSERT(buf.size() == 0);
    return true;
}

bool TestEzBinBuf_Reserve() {
    ez::BinBuf buf;
    buf.reserve(100);
    CTEST_ASSERT(true);  // pas de crash = ok
    return true;
}

bool TestEzBinBuf_OperatorAt() {
    ez::BinBuf buf;
    buf.writeValueLE<uint8_t>(42);
    CTEST_ASSERT(buf[0] == 42);
    CTEST_ASSERT(buf.at(0) == 42);
    buf.at(0) = 99;
    CTEST_ASSERT(buf[0] == 99);
    return true;
}

bool TestEzBinBuf_OutOfRange() {
    ez::BinBuf buf;
    bool threw = false;
    try {
        auto _ = buf[0];
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CTEST_ASSERT(threw);

    threw = false;
    try {
        buf.at(0) = 1;
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CTEST_ASSERT(threw);

    threw = false;
    size_t pos = 0;
    try {
        buf.readValueLE<uint32_t>(pos);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CTEST_ASSERT(threw);

    return true;
}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzBinBuf(const std::string& vTest) {
    IfTestExist(TestEzBinBuf_WriteReadValueLE);
    else IfTestExist(TestEzBinBuf_WriteReadValueBE);
    else IfTestExist(TestEzBinBuf_WriteReadArrayLE);
    else IfTestExist(TestEzBinBuf_WriteReadArrayBE);
    else IfTestExist(TestEzBinBuf_SetDatasGetDatas);
    else IfTestExist(TestEzBinBuf_ClearSize);
    else IfTestExist(TestEzBinBuf_Reserve);
    else IfTestExist(TestEzBinBuf_OperatorAt);
    else IfTestExist(TestEzBinBuf_OutOfRange);
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
