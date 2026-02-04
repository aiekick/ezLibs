#include <ezlibs/ezCTest.hpp>
#include <ezlibs/ezGeo/ezDemAsc.hpp>
#include <ezlibs/ezGeo/ezDemBin.hpp>
#include <ezlibs/ezGeo/ezHgt.hpp>
#include <ezlibs/ezGeo/ezShp.hpp>
#include <string>
#include <vector>

// Disable noisy conversion warnings to match your style
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Test ezDemAsc load functionality
bool TestEzFormats_DemAsc_Load_ValidFile() {
    ez::geo::DemAsc demAsc;

    // Create a valid DEM ASCII buffer
    std::string buffer =
        "nrows 3\n"
        "ncols 3\n"
        "xllcorner 1.0\n"
        "yllcorner 2.0\n"
        "cellsize 30\n"
        "NODATA_value -9999\n"
        "100 200 300\n"
        "150 250 350\n"
        "200 300 400\n";

    bool result = demAsc.load("N48E002.asc", buffer);
    CTEST_ASSERT(result);
    CTEST_ASSERT(demAsc.isValid());

    const auto& datas = demAsc.getDatas();
    CTEST_ASSERT(datas.nLats == 3);
    CTEST_ASSERT(datas.nLons == 3);
    CTEST_ASSERT(ez::isEqual(datas.xllcorner, 1.0));
    CTEST_ASSERT(ez::isEqual(datas.yllcorner, 2.0));
    CTEST_ASSERT(datas.cellsize == 30);
    CTEST_ASSERT(datas.NODATA_value == -9999);

    return true;
}

// Test ezDemAsc save functionality
bool TestEzFormats_DemAsc_Save_RoundTrip() {
    ez::geo::DemAsc demAsc;

    // Load original
    std::string buffer =
        "nrows 2\n"
        "ncols 2\n"
        "xllcorner 1.5\n"
        "yllcorner 2.5\n"
        "cellsize 30\n"
        "NODATA_value -9999\n"
        "100 200\n"
        "150 250\n";

    CTEST_ASSERT(demAsc.load("N48E002.asc", buffer));

    // Save to new buffer
    std::string savedBuffer;
    CTEST_ASSERT(demAsc.save(savedBuffer));
    CTEST_ASSERT(!savedBuffer.empty());

    // Load saved buffer and verify
    ez::geo::DemAsc demAsc2;
    CTEST_ASSERT(demAsc2.load("N48E002.asc", savedBuffer));

    const auto& datas1 = demAsc.getDatas();
    const auto& datas2 = demAsc2.getDatas();

    CTEST_ASSERT(datas1.nLats == datas2.nLats);
    CTEST_ASSERT(datas1.nLons == datas2.nLons);
    CTEST_ASSERT(ez::isEqual(datas1.xllcorner, datas2.xllcorner));
    CTEST_ASSERT(ez::isEqual(datas1.yllcorner, datas2.yllcorner));

    return true;
}

// Test ezDemAsc with invalid filename
bool TestEzFormats_DemAsc_Load_InvalidFilename() {
    ez::geo::DemAsc demAsc;
    std::string buffer = "nrows 2\nncols 2\n";

    // Invalid filename should fail
    bool result = demAsc.load("invalid.asc", buffer);
    CTEST_ASSERT(!result);

    return true;
}

// Test ezDemAsc with empty buffer
bool TestEzFormats_DemAsc_Load_EmptyBuffer() {
    ez::geo::DemAsc demAsc;
    std::string buffer = "";

    bool result = demAsc.load("N48E002.asc", buffer);
    CTEST_ASSERT(!result);

    return true;
}

////////////////////////////////////////////////////////////////////////////
// ezDemBin Tests
////////////////////////////////////////////////////////////////////////////

// Test ezDemBin load functionality
bool TestEzFormats_DemBin_Load_ValidFile() {
    ez::geo::DemBin demBin;

    // Create a valid binary DEM buffer
    std::vector<uint8_t> bytes;
    ez::BinBuf binBuf;

    // Write date (16 bytes): "01/01/1970 00/00"
    std::string date = "01/01/1970 00/00";
    binBuf.writeArrayBE(date.data(), 16);

    // Write metadata
    uint32_t resLat = 3600;
    uint32_t resLon = 3600;
    float fLat = 48.0f;
    float fLon = 2.0f;
    uint32_t nLats = 2;
    uint32_t nLons = 2;

    binBuf.writeValueBE<uint32_t>(resLat);
    binBuf.writeValueBE<uint32_t>(resLon);
    binBuf.writeValueBE<float>(fLat);
    binBuf.writeValueBE<float>(fLon);
    binBuf.writeValueBE<uint32_t>(nLats);
    binBuf.writeValueBE<uint32_t>(nLons);

    // Write elevation data
    std::vector<int16_t> row1 = {100, 200};
    std::vector<int16_t> row2 = {150, 250};
    binBuf.writeArrayBE<int16_t>(row1.data(), row1.size());
    binBuf.writeArrayBE<int16_t>(row2.data(), row2.size());

    bytes = binBuf.getDatas();

    bool result = demBin.load("N48E002.dem", bytes);
    CTEST_ASSERT(result);
    CTEST_ASSERT(demBin.isValid());

    const auto& datas = demBin.getDatas();
    CTEST_ASSERT(datas.nLats == 2);
    CTEST_ASSERT(datas.nLons == 2);
    CTEST_ASSERT(datas.resLat == 3600);
    CTEST_ASSERT(datas.resLon == 3600);

    return true;
}

// Test ezDemBin save functionality
bool TestEzFormats_DemBin_Save_RoundTrip() {
    ez::geo::DemBin demBin;

    // Setup data manually
    auto& datas = demBin.getDatasRef();
    datas.latStr = "N48";
    datas.lonStr = "E002";
    datas.lat = 48;
    datas.lon = 2;
    datas.resLat = 3600;
    datas.resLon = 3600;
    datas.fLat = 48.0f;
    datas.fLon = 2.0f;
    datas.nLats = 2;
    datas.nLons = 2;

    // Set tile data
    auto& tileDatas = datas.tile.getDatasRef();
    tileDatas.resize(2);
    tileDatas[0] = {100, 200};
    tileDatas[1] = {150, 250};

    // Save
    std::vector<uint8_t> bytes;
    CTEST_ASSERT(demBin.save(bytes));
    CTEST_ASSERT(!bytes.empty());

    // Load and verify
    ez::geo::DemBin demBin2;
    CTEST_ASSERT(demBin2.load("N48E002.dem", bytes));

    const auto& datas2 = demBin2.getDatas();
    CTEST_ASSERT(datas2.nLats == datas.nLats);
    CTEST_ASSERT(datas2.nLons == datas.nLons);

    return true;
}

// Test ezDemBin with invalid filename
bool TestEzFormats_DemBin_Load_InvalidFilename() {
    ez::geo::DemBin demBin;
    std::vector<uint8_t> bytes = {0x01, 0x02, 0x03};

    bool result = demBin.load("invalid.dem", bytes);
    CTEST_ASSERT(!result);

    return true;
}

// Test ezDemBin with empty buffer
bool TestEzFormats_DemBin_Load_EmptyBuffer() {
    ez::geo::DemBin demBin;
    std::vector<uint8_t> bytes;

    bool result = demBin.load("N48E002.dem", bytes);
    CTEST_ASSERT(!result);

    return true;
}

////////////////////////////////////////////////////////////////////////////
// ezHgt Tests
////////////////////////////////////////////////////////////////////////////

// Test ezHgt load functionality with SRTM1 size (3601x3601)
bool TestEzFormats_Hgt_Load_SRTM1() {
    ez::geo::hgt hgtFile;

    // SRTM1 has 3601x3601 samples = 12,967,201 int16 values = 25,934,402 bytes
    const size_t srtm1Size = 3601 * 3601 * 2;
    std::vector<uint8_t> bytes(srtm1Size, 0);

    // Fill with some test data (big-endian int16)
    ez::BinBuf binBuf;
    for (int i = 0; i < 3601 * 3601; ++i) {
        binBuf.writeValueBE<int16_t>(static_cast<int16_t>(100 + (i % 100)));
    }
    bytes = binBuf.getDatas();

    bool result = hgtFile.load("N48E002.hgt", bytes);
    CTEST_ASSERT(result);
    CTEST_ASSERT(hgtFile.isValid());

    const auto& datas = hgtFile.getDatas();
    CTEST_ASSERT(datas.latStr == "N48");
    CTEST_ASSERT(datas.lonStr == "E002");

    return true;
}

// Test ezHgt load functionality with SRTM3 size (1201x1201)
bool TestEzFormats_Hgt_Load_SRTM3() {
    ez::geo::hgt hgtFile;

    // SRTM3 has 1201x1201 samples = 1,442,401 int16 values = 2,884,802 bytes
    const size_t srtm3Size = 1201 * 1201 * 2;
    std::vector<uint8_t> bytes(srtm3Size, 0);

    // Fill with some test data
    ez::BinBuf binBuf;
    for (int i = 0; i < 1201 * 1201; ++i) {
        binBuf.writeValueBE<int16_t>(static_cast<int16_t>(50 + (i % 50)));
    }
    bytes = binBuf.getDatas();

    bool result = hgtFile.load("N48E002.hgt", bytes);
    CTEST_ASSERT(result);
    CTEST_ASSERT(hgtFile.isValid());

    return true;
}

// Test ezHgt save functionality
bool TestEzFormats_Hgt_Save_RoundTrip() {
    ez::geo::hgt hgtFile;

    // Setup small test data (1201x1201 for SRTM3)
    auto& datas = hgtFile.getDatasRef();
    datas.latStr = "N48";
    datas.lonStr = "E002";
    datas.lat = 48;
    datas.lon = 2;

    // Create small tile for testing (3x3 instead of 1201x1201)
    auto& tileDatas = datas.tile.getDatasRef();
    tileDatas.resize(3);
    for (int i = 0; i < 3; ++i) {
        tileDatas[i] = {100, 200, 300};
    }

    // Save
    std::vector<uint8_t> bytes;
    CTEST_ASSERT(hgtFile.save(bytes));
    CTEST_ASSERT(!bytes.empty());
    CTEST_ASSERT(bytes.size() == 3 * 3 * 2);  // 3x3 int16 values

    return true;
}

// Test ezHgt with empty buffer
bool TestEzFormats_Hgt_Load_EmptyBuffer() {
    ez::geo::hgt hgtFile;
    std::vector<uint8_t> bytes;

    bool result = hgtFile.load("N48E002.hgt", bytes);
    CTEST_ASSERT(!result);

    return true;
}

////////////////////////////////////////////////////////////////////////////
// ezShp Tests
////////////////////////////////////////////////////////////////////////////

// Test ezShp load functionality with valid bytes (polygon)
bool TestEzFormats_Shp_Load_ValidFile() {
    ez::Shp shp;

    // Create a minimal valid shapefile with one polygon
    // Based on https://en.wikipedia.org/wiki/Shapefile format
    ez::BinBuf binBuf;

    // Header (100 bytes)
    binBuf.writeValueBE<uint32_t>(9994);  // Magic number
    for (int i = 0; i < 5; i++) {
        binBuf.writeValueBE<uint32_t>(0);  // Padding (20 bytes)
    }
    binBuf.writeValueBE<uint32_t>(94);  // File length in 16-bit words (header + record)
    binBuf.writeValueLE<uint32_t>(1000);  // Version
    binBuf.writeValueLE<uint32_t>(5);  // Shape type: Polygon
    binBuf.writeValueLE<double>(0.0);  // xmin
    binBuf.writeValueLE<double>(0.0);  // ymin
    binBuf.writeValueLE<double>(10.0);  // xmax
    binBuf.writeValueLE<double>(10.0);  // ymax
    binBuf.writeValueLE<double>(0.0);  // zmin
    binBuf.writeValueLE<double>(0.0);  // zmax
    binBuf.writeValueLE<double>(0.0);  // mmin
    binBuf.writeValueLE<double>(0.0);  // mmax

    // Record header
    binBuf.writeValueBE<uint32_t>(1);  // Record number
    binBuf.writeValueBE<uint32_t>(42);  // Content length in 16-bit words

    // Record content (Polygon)
    binBuf.writeValueLE<uint32_t>(5);  // Shape type: Polygon
    binBuf.writeValueLE<double>(0.0);  // Box xmin
    binBuf.writeValueLE<double>(0.0);  // Box ymin
    binBuf.writeValueLE<double>(10.0);  // Box xmax
    binBuf.writeValueLE<double>(10.0);  // Box ymax
    binBuf.writeValueLE<uint32_t>(1);  // NumParts
    binBuf.writeValueLE<uint32_t>(4);  // NumPoints
    binBuf.writeValueLE<uint32_t>(0);  // Parts[0] = 0

    // Points (4 points forming a square)
    binBuf.writeValueLE<double>(0.0);  // x0
    binBuf.writeValueLE<double>(0.0);  // y0
    binBuf.writeValueLE<double>(10.0);  // x1
    binBuf.writeValueLE<double>(0.0);  // y1
    binBuf.writeValueLE<double>(10.0);  // x2
    binBuf.writeValueLE<double>(10.0);  // y2
    binBuf.writeValueLE<double>(0.0);  // x3
    binBuf.writeValueLE<double>(10.0);  // y3

    std::vector<uint8_t> bytes = binBuf.getDatas();

    bool result = shp.loadBytes(bytes);
    CTEST_ASSERT(result);
    CTEST_ASSERT(shp.isValid());

    const auto& datas = shp.getDatas();
    CTEST_ASSERT(datas.header.beMagic == 9994);
    CTEST_ASSERT(datas.header.version == 1000);
    CTEST_ASSERT(datas.header.shapeType == ez::Shp::ShapeType::Polygon);
    CTEST_ASSERT(!datas.records.empty());
    CTEST_ASSERT(datas.records[0].shapeType == ez::Shp::ShapeType::Polygon);
    CTEST_ASSERT(datas.records[0].polygons.size() == 1);
    CTEST_ASSERT(datas.records[0].polygons[0].numParts == 1);
    CTEST_ASSERT(datas.records[0].polygons[0].numPoints == 4);

    return true;
}

// Test ezShp load functionality with loadBytes
bool TestEzFormats_Shp_Load_ValidBytes() {
    ez::Shp shp;

    // Create a minimal valid shapefile with one polyline
    ez::BinBuf binBuf;

    // Header (100 bytes)
    binBuf.writeValueBE<uint32_t>(9994);  // Magic number
    for (int i = 0; i < 5; i++) {
        binBuf.writeValueBE<uint32_t>(0);  // Padding
    }
    binBuf.writeValueBE<uint32_t>(94);  // File length
    binBuf.writeValueLE<uint32_t>(1000);  // Version
    binBuf.writeValueLE<uint32_t>(3);  // Shape type: PolyLine
    binBuf.writeValueLE<double>(0.0);  // xmin
    binBuf.writeValueLE<double>(0.0);  // ymin
    binBuf.writeValueLE<double>(5.0);  // xmax
    binBuf.writeValueLE<double>(5.0);  // ymax
    binBuf.writeValueLE<double>(0.0);  // zmin
    binBuf.writeValueLE<double>(0.0);  // zmax
    binBuf.writeValueLE<double>(0.0);  // mmin
    binBuf.writeValueLE<double>(0.0);  // mmax

    // Record
    binBuf.writeValueBE<uint32_t>(1);  // Record number
    binBuf.writeValueBE<uint32_t>(42);  // Content length
    binBuf.writeValueLE<uint32_t>(3);  // Shape type: PolyLine
    binBuf.writeValueLE<double>(0.0);  // Box xmin
    binBuf.writeValueLE<double>(0.0);  // Box ymin
    binBuf.writeValueLE<double>(5.0);  // Box xmax
    binBuf.writeValueLE<double>(5.0);  // Box ymax
    binBuf.writeValueLE<uint32_t>(1);  // NumParts
    binBuf.writeValueLE<uint32_t>(4);  // NumPoints
    binBuf.writeValueLE<uint32_t>(0);  // Parts[0]
    binBuf.writeValueLE<double>(0.0);  // x0
    binBuf.writeValueLE<double>(0.0);  // y0
    binBuf.writeValueLE<double>(5.0);  // x1
    binBuf.writeValueLE<double>(0.0);  // y1
    binBuf.writeValueLE<double>(5.0);  // x2
    binBuf.writeValueLE<double>(5.0);  // y2
    binBuf.writeValueLE<double>(0.0);  // x3
    binBuf.writeValueLE<double>(5.0);  // y3

    std::vector<uint8_t> bytes = binBuf.getDatas();

    bool result = shp.loadBytes(bytes);
    CTEST_ASSERT(result);
    CTEST_ASSERT(shp.isValid());

    const auto& datas = shp.getDatas();
    CTEST_ASSERT(datas.header.beMagic == 9994);
    CTEST_ASSERT(datas.header.version == 1000);
    CTEST_ASSERT(datas.header.shapeType == ez::Shp::ShapeType::PolyLine);
    CTEST_ASSERT(!datas.records.empty());
    CTEST_ASSERT(datas.records[0].polyLines.size() == 1);

    return true;
}

// Test ezShp with empty buffer
bool TestEzFormats_Shp_Load_EmptyBuffer() {
    ez::Shp shp;
    std::vector<uint8_t> bytes;

    bool result = shp.loadBytes(bytes);
    CTEST_ASSERT(!result);
    CTEST_ASSERT(!shp.isValid());

    return true;
}

// Test ezShp with invalid file extension
bool TestEzFormats_Shp_Load_InvalidExtension() {
    ez::Shp shp;

    bool result = shp.loadFile("test.txt");
    CTEST_ASSERT(!result);

    return true;
}

// Test ezShp Point structure
bool TestEzFormats_Shp_Point_Creation() {
    ez::Shp::Point pt;
    pt.x = 10.5;
    pt.y = 20.3;

    CTEST_ASSERT(ez::isEqual(pt.x, 10.5));
    CTEST_ASSERT(ez::isEqual(pt.y, 20.3));

    return true;
}

// Test ezShp PointZ structure
bool TestEzFormats_Shp_PointZ_Creation() {
    ez::Shp::PointZ ptz;
    ptz.x = 10.5;
    ptz.y = 20.3;
    ptz.z = 30.7;
    ptz.m = 40.1;

    CTEST_ASSERT(ez::isEqual(ptz.x, 10.5));
    CTEST_ASSERT(ez::isEqual(ptz.y, 20.3));
    CTEST_ASSERT(ez::isEqual(ptz.z, 30.7));
    CTEST_ASSERT(ez::isEqual(ptz.m, 40.1));

    return true;
}

// Test ezShp PointM structure
bool TestEzFormats_Shp_PointM_Creation() {
    ez::Shp::PointM ptm;
    ptm.x = 10.5;
    ptm.y = 20.3;
    ptm.m = 15.2;

    CTEST_ASSERT(ez::isEqual(ptm.x, 10.5));
    CTEST_ASSERT(ez::isEqual(ptm.y, 20.3));
    CTEST_ASSERT(ez::isEqual(ptm.m, 15.2));

    return true;
}

// Test ezShp MultiPoint structure
bool TestEzFormats_Shp_MultiPoint_Creation() {
    ez::Shp::MultiPoint mp;
    mp.box = {0.0, 0.0, 100.0, 100.0};
    mp.numPoints = 3;
    mp.points.resize(3);
    mp.points[0] = {10.0, 20.0};
    mp.points[1] = {30.0, 40.0};
    mp.points[2] = {50.0, 60.0};

    CTEST_ASSERT(mp.numPoints == 3);
    CTEST_ASSERT(mp.points.size() == 3);
    CTEST_ASSERT(ez::isEqual(mp.points[0].x, 10.0));
    CTEST_ASSERT(ez::isEqual(mp.points[1].y, 40.0));

    return true;
}

// Test ezShp PolyTruc structure (base for polyline/polygon)
bool TestEzFormats_Shp_PolyTruc_Creation() {
    ez::Shp::PolyTruc poly;
    poly.box = {0.0, 0.0, 100.0, 100.0};
    poly.numParts = 1;
    poly.numPoints = 4;
    poly.parts = {0};  // Start of first part
    poly.points = {
        {0.0, 0.0},
        {100.0, 0.0},
        {100.0, 100.0},
        {0.0, 100.0}
    };

    CTEST_ASSERT(poly.numParts == 1);
    CTEST_ASSERT(poly.numPoints == 4);
    CTEST_ASSERT(poly.parts.size() == 1);
    CTEST_ASSERT(poly.points.size() == 4);

    return true;
}

// Test ezShp ShapeType enum values
bool TestEzFormats_Shp_ShapeType_Values() {
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::Null) == 0);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::Point) == 1);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::PolyLine) == 3);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::Polygon) == 5);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::MultiPoint) == 8);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::PointZ) == 11);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::PolyLineZ) == 13);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::ShapeType::PolygonZ) == 15);

    return true;
}

// Test ezShp PartType enum values
bool TestEzFormats_Shp_PartType_Values() {
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::PartType::TriangleStrip) == 0);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::PartType::TriangleFan) == 1);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::PartType::OuterRing) == 2);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::PartType::InnerRing) == 3);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::PartType::FirstRing) == 4);
    CTEST_ASSERT(static_cast<uint32_t>(ez::Shp::PartType::Ring) == 5);

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzFormats(const std::string& vTest) {
    // ezDemAsc tests
    IfTestExist(TestEzFormats_DemAsc_Load_ValidFile);
    else IfTestExist(TestEzFormats_DemAsc_Save_RoundTrip);
    else IfTestExist(TestEzFormats_DemAsc_Load_InvalidFilename);
    else IfTestExist(TestEzFormats_DemAsc_Load_EmptyBuffer);

    // ezDemBin tests
    IfTestExist(TestEzFormats_DemBin_Load_ValidFile);
    else IfTestExist(TestEzFormats_DemBin_Save_RoundTrip);
    else IfTestExist(TestEzFormats_DemBin_Load_InvalidFilename);
    else IfTestExist(TestEzFormats_DemBin_Load_EmptyBuffer);

    // ezHgt tests
    IfTestExist(TestEzFormats_Hgt_Load_SRTM1);
    else IfTestExist(TestEzFormats_Hgt_Load_SRTM3);
    else IfTestExist(TestEzFormats_Hgt_Save_RoundTrip);
    else IfTestExist(TestEzFormats_Hgt_Load_EmptyBuffer);

    // ezShp tests
    IfTestExist(TestEzFormats_Shp_Load_ValidFile);
    else IfTestExist(TestEzFormats_Shp_Load_ValidBytes);
    else IfTestExist(TestEzFormats_Shp_Load_EmptyBuffer);
    else IfTestExist(TestEzFormats_Shp_Load_InvalidExtension);
    else IfTestExist(TestEzFormats_Shp_Point_Creation);
    else IfTestExist(TestEzFormats_Shp_PointZ_Creation);
    else IfTestExist(TestEzFormats_Shp_PointM_Creation);
    else IfTestExist(TestEzFormats_Shp_MultiPoint_Creation);
    else IfTestExist(TestEzFormats_Shp_PolyTruc_Creation);
    else IfTestExist(TestEzFormats_Shp_ShapeType_Values);
    else IfTestExist(TestEzFormats_Shp_PartType_Values);

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
