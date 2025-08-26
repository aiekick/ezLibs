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

// ezShp is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/* ShapeFile reader/writer : https://en.wikipedia.org/wiki/Shapefile
Support only (for the moment) :
 - polygon
 - polylines
 - read file/bytes
*/

#include <string>
#include <vector>
#include <array>
#include "../ezBinBuf.hpp"
#include "../ezFile.hpp"

namespace ez {

class Shp {
public:
    enum class ShapeType : uint32_t {
        Null = 0,
        Point = 1,
        PolyLine = 3,
        Polygon = 5,
        MultiPoint = 8,
        PointZ = 11,
        PolyLineZ = 13,
        PolygonZ = 15,
        MultiPointZ = 18,
        PointM = 21,
        PolyLineM = 23,
        PolygonM = 25,
        MultiPointM = 28,
        MultiPatch = 32
    };

    struct NullShape {};

    struct Point {
        double x;
        double y;
    };

    struct MultiPoint {
        std::array<double, 4> box;
        uint32_t numPoints;
        std::vector<Point> points;  // numPoints
    };

    struct PolyTruc {
        std::array<double, 4> box;
        uint32_t numParts;
        uint32_t numPoints;
        std::vector<uint32_t> parts;  // numParts
        std::vector<Point> points;    // numPoints
    };

    struct PointM {
        double x;
        double y;
        double m;
    };

    struct MultiPointM {
        std::array<double, 4> box;
        uint32_t numPoints;
        std::vector<Point> points;  // numPoints
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    struct PolyLineM {
        PolyTruc truc;
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    struct PolygonM {
        PolyTruc truc;
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    struct PointZ {
        double x;
        double y;
        double z;
        double m;
    };

    struct MultiPointZ {
        std::array<double, 4> box;
        uint32_t numPoints;
        std::vector<Point> points;  // numPoints
        std::array<double, 2U> z_range;
        std::vector<double> z_arrays;  // numPoints
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    struct PolyLineZ {
        PolyTruc truc;
        std::array<double, 2U> z_range;
        std::vector<double> z_arrays;  // numPoints
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    struct PolygonZ {
        PolyTruc truc;
        std::array<double, 2U> z_range;
        std::vector<double> z_arrays;  // numPoints
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    enum class PartType : uint32_t {
        TriangleStrip = 0,
        TriangleFan = 1,
        OuterRing = 2,
        InnerRing = 3,
        FirstRing = 4,
        Ring = 5,
    };

    struct MultiPatch {
        std::array<double, 4> box;
        uint32_t numParts;
        uint32_t numPoints;
        std::vector<uint32_t> parts;      // numParts
        std::vector<PartType> partTypes;  // numParts;
        std::vector<Point> points;
        std::array<double, 2U> z_range;
        std::vector<double> z_arrays;  // numPoints
        std::array<double, 2U> m_range;
        std::vector<double> m_arrays;  // numPoints
    };

    struct Header {
        uint32_t beMagic;                   // 9994
        std::array<uint8_t, 20> bePadding;  // unused
        uint32_t beFileLength;              // in 16-bit words (including header)
        uint32_t version;                   // 1000
        ShapeType shapeType;
        double bb_xmin;
        double bb_ymin;
        double bb_xmax;
        double bb_ymax;
        double bb_zmin;
        double bb_zmax;
        double bb_mmin;
        double bb_mmax;
    };

    struct RecordHeader {
        uint32_t beRecordNumber;   // starting at 1
        uint32_t beContentLength;  // in 16-bit words
    };

    struct Record {
        RecordHeader header;
        ShapeType shapeType;
        std::vector<NullShape> nullShapes;
        std::vector<Point> points;
        std::vector<PolyTruc> polyLines;
        std::vector<PolyTruc> polygons;
        std::vector<MultiPoint> multiPoints;
        std::vector<PointZ> pointZs;
        std::vector<PolyLineZ> polyLineZs;
        std::vector<PolygonZ> polygonZs;
        std::vector<MultiPointZ> multiPointZs;
        std::vector<PointM> pointMs;
        std::vector<PolyLineM> polyLineMs;
        std::vector<PolygonM> polygonMs;
        std::vector<MultiPointM> multiPointMs;
        std::vector<MultiPatch> multiPatchs;
    };

    struct ShpDatas {
        Header header;
        std::vector<Record> records;
    };

    struct ShpInfos {
        std::string filePathName;
        bool valid{};
    };

private:
    ShpDatas m_datas{};
    ShpInfos m_infos{};

public:
    bool loadFile(const std::string& vFilePathName) {
        m_infos.filePathName = vFilePathName;
        const auto ps = ez::file::parsePathFileName(m_infos.filePathName);
        if (ps.isOk && !ps.ext.empty()) {
            if (ps.ext == "shp") {
                return loadBytes(ez::file::loadFileToBin(m_infos.filePathName));
            }
        }
        return false;
    }

    bool loadBytes(const std::vector<uint8_t>& vCode) {
        if (vCode.empty()) {
            return false;
        }
        m_infos = {};

        ez::BinBuf binBuf;
        binBuf.setDatas(vCode);
        size_t pos = 0;  // on saute la date
        // read header
        m_datas = {};
        m_datas.header.beMagic = binBuf.readValueBE<uint32_t>(pos);
        pos += 20;
        m_datas.header.beFileLength = binBuf.readValueBE<uint32_t>(pos);
        m_datas.header.version = binBuf.readValueLE<uint32_t>(pos);
        m_datas.header.shapeType = static_cast<ShapeType>(binBuf.readValueLE<uint32_t>(pos));
        m_datas.header.bb_xmin = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_ymin = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_xmax = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_ymax = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_zmin = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_zmax = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_mmin = binBuf.readValueLE<double>(pos);
        m_datas.header.bb_mmax = binBuf.readValueLE<double>(pos);
        // read record
        const size_t countbytes = size_t(m_datas.header.beFileLength * 2U);
        while (pos < countbytes) {
            Record record;
            record.header.beRecordNumber = binBuf.readValueBE<uint32_t>(pos);
            record.header.beContentLength = binBuf.readValueBE<uint32_t>(pos);
            record.shapeType = static_cast<ShapeType>(binBuf.readValueLE<uint32_t>(pos));
            if (record.shapeType == ShapeType::PolyLine ||  //
                record.shapeType == ShapeType::Polygon) {
                PolyTruc polytruc;
                binBuf.readArrayLE(pos, polytruc.box.data(), polytruc.box.size());
                polytruc.numParts = binBuf.readValueLE<uint32_t>(pos);
                polytruc.numPoints = binBuf.readValueLE<uint32_t>(pos);
                polytruc.parts.resize(polytruc.numParts);
                binBuf.readArrayLE(pos, polytruc.parts.data(), polytruc.parts.size());
                polytruc.points.resize(polytruc.numPoints);
                for (auto& point : polytruc.points) {
                    point.x = binBuf.readValueLE<double>(pos);
                    point.y = binBuf.readValueLE<double>(pos);
                }
                if (record.shapeType == ShapeType::PolyLine) {
                    record.polyLines.push_back(polytruc);
                } else if (record.shapeType == ShapeType::Polygon) {
                    record.polygons.push_back(polytruc);
                }
            }
            m_datas.records.push_back(record);
        }
        if (!m_datas.records.empty()) {
            m_infos.valid = true;
        }
        return isValid();
    }

    bool getBytes(std::vector<uint8_t>& vOutBuffer) const {
        // todo
        return false;
    }
    
    bool saveFile(const std::string& vFilePathName) const {
        // todo
        return false;
    }

    bool isValid() const { return m_infos.valid; }

    const ShpInfos& getInfos() const { return m_infos; }

    const ShpDatas& getDatas() const { return m_datas; }
};

}  // namespace ez
