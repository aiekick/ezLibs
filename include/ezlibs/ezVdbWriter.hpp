#pragma once

/*
MIT License

Copyright (c) 2022-2024 Stephane Cuillerdier (aka aiekick)

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

// ezVdb is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <unordered_map>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <cstring>
#include <limits>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <map>

#include "ezMath.hpp"

namespace ez {
namespace file {
namespace vdb {

static void write_ptr(FILE* fp, const void* data, size_t elementSize, size_t count = 1) {
    fwrite(data, elementSize, count, fp);
}
static void write_ptr(FILE* fp, void* data, size_t elementSize, size_t count = 1) {
    fwrite(data, elementSize, count, fp);
}

template <typename T>
void write_data(FILE* fp, T data) {
    write_ptr(fp, &data, sizeof(T));
}

template <typename T>
void write_data_arr(FILE* fp, T* data, size_t count) {
    write_ptr(fp, data, sizeof(T), count);
}

template <typename T>
void write_data_arr(FILE* fp, const T* data, size_t count) {
    write_ptr(fp, data, sizeof(T), count);
}

static void write_string(FILE* fp, const std::string& str) {
    write_ptr(fp, str.data(), sizeof(uint8_t), str.size());
}
inline void write_vec3i(FILE* fp, const std::array<int32_t, 3>& data) {
    write_data_arr<int32_t>(fp, data.data(), 3);
}

inline void write_name(FILE* fp, const std::string& name) {
    write_data<uint32_t>(fp, (uint32_t)name.size());
    write_string(fp, name);
}

inline void write_meta_string(FILE* fp, const std::string& name, const std::string& str) {
    write_name(fp, name);
    write_name(fp, "string");
    write_name(fp, str);
}

inline void write_meta_bool(FILE* fp, const std::string& name, bool flag) {
    write_name(fp, name);
    write_name(fp, "bool");
    write_data<uint32_t>(fp, 1);  // one byte is used to store the bool
    write_data<uint8_t>(fp, flag ? 1 : 0);
}

inline void write_meta_vec3i(FILE* fp, const std::string& name, const std::array<int32_t, 3>& data) {
    write_name(fp, name);
    write_name(fp, "vec3i");
    write_data<uint32_t>(fp, 12U);  // 12 bytes (4 bytes * 3) are used to store the vec3i
    write_vec3i(fp, data);
}

class ATree {
protected:
    dAABBCC m_Volume;
    std::string m_Name;

protected:
    virtual bool addVoxel(uint32_t vX, uint32_t vY, uint32_t vZ, void* vDatas, size_t vByteSize, size_t vCount) = 0;

    static uint32_t getBitIndex4(uint32_t vX, uint32_t vY, uint32_t vZ) {
        const auto& x = vX & (uint32_t)(4096 - 1);
        const auto& y = vY & (uint32_t)(4096 - 1);
        const auto& z = vZ & (uint32_t)(4096 - 1);
        uint32_t idx_3d[3] = {x >> 7, y >> 7, z >> 7};
        uint64_t idx = idx_3d[2] | (idx_3d[1] << 5) | (idx_3d[0] << 10);
        return static_cast<uint32_t>(idx);
    }

    static uint32_t getBitIndex3(uint32_t vX, uint32_t vY, uint32_t vZ) {
        const auto& x = vX & (uint32_t)(128 - 1);
        const auto& y = vY & (uint32_t)(128 - 1);
        const auto& z = vZ & (uint32_t)(128 - 1);
        uint32_t idx_3d[3] = {x >> 3, y >> 3, z >> 3};
        uint64_t idx = idx_3d[2] | (idx_3d[1] << 4) | (idx_3d[0] << 8);
        return static_cast<uint32_t>(idx);
    }

    static uint32_t getBitIndex0(uint32_t vX, uint32_t vY, uint32_t vZ) {
        const auto& x = vX & (uint32_t)(8 - 1);
        const auto& y = vY & (uint32_t)(8 - 1);
        const auto& z = vZ & (uint32_t)(8 - 1);
        uint32_t idx_3d[3] = {x >> 0, y >> 0, z >> 0};
        uint64_t idx = idx_3d[2] | (idx_3d[1] << 3) | (idx_3d[0] << 6);
        return static_cast<uint32_t>(idx);
    }

    static int32_t count_trailing_zeros(uint64_t value) {
        int32_t count = 0;
        while ((value & 1) == 0 && value != 0) {
            ++count;
            value >>= 1;
        }
        return count;
    }

public:
    ATree(const std::string& vName) : m_Name(vName) {
    }
    virtual ~ATree() = default;

    virtual void write(FILE* vFp) = 0;
};

template <typename TType, size_t TCount>
class VdbTree : public ATree {
private:
    struct Node3 {
        uint64_t mask[8] = {};
        std::array<TType, TCount> data[512] = {};  // data
    };

    struct Node4 {
        uint64_t mask[64] = {};
        std::map<uint32_t, Node3> nodes;  // loc, node
    };

    struct Node5 {
        uint64_t mask[512] = {};
        std::map<uint32_t, Node4> nodes;  // loc, node
    };

    void writeNode4EmptyHeader(FILE* fp, Node4* node) {
        write_data_arr<uint64_t>(fp, node->mask, 64);
        static std::vector<uint64_t> mask_arr_uint64_empty(64);  // we dont use a std::array for not increase the bin size
        write_data_arr<uint64_t>(fp, mask_arr_uint64_empty.data(), mask_arr_uint64_empty.size());
        write_data<uint8_t>(fp, 6);
        static std::vector<float> mask_arr_float_empty(4096);
        write_data_arr<float>(fp, mask_arr_float_empty.data(), mask_arr_float_empty.size());
    }

    void writeNode5EmptyHeader(FILE* fp, Node5* node) {
        write_vec3i(fp, {0, 0, 0});
        write_data_arr<uint64_t>(fp, node->mask, 512);
        static std::vector<uint64_t> mask_arr_uint64_empty(512);  // we dont use a std::array for not increase the bin size
        write_data_arr<uint64_t>(fp, mask_arr_uint64_empty.data(), mask_arr_uint64_empty.size());
        write_data<uint8_t>(fp, 6);
        static std::vector<float> mask_arr_float_empty(32768);
        write_data_arr<float>(fp, mask_arr_float_empty.data(), mask_arr_float_empty.size());
    }

    void writeTree(FILE* fp) {
        write_data<uint32_t>(fp, 1);
        write_data<float>(fp, 0);
        write_data<uint32_t>(fp, 0);
        write_data<uint32_t>(fp, 1);
        auto& nodes5Ref = m_Nodes;
        writeNode5EmptyHeader(fp, &nodes5Ref);
        size_t word5_idx = 0;
        for (auto word5 : nodes5Ref.mask) {
            const auto& base_bit_4_idx = ((uint32_t)word5_idx++) * 64;
            for (; word5 != 0; word5 &= word5 - 1) {
                const auto& bit_4_index = base_bit_4_idx + (uint32_t)count_trailing_zeros(word5);
                auto& nodes4Ref = nodes5Ref.nodes.at(bit_4_index);
                writeNode4EmptyHeader(fp, &nodes4Ref);
                size_t word4_idx = 0;
                for (auto word4 : nodes4Ref.mask) {
                    const auto& base_bit_3_idx = ((uint32_t)word4_idx++) * 64;
                    for (; word4 != 0; word4 &= word4 - 1) {
                        const auto& bit_3_index = base_bit_3_idx + (uint32_t)count_trailing_zeros(word4);
                        const auto& nodes3Ref = nodes4Ref.nodes.at(bit_3_index);
                        write_data_arr<uint64_t>(fp, nodes3Ref.mask, 8);
                    }
                }
            }
        }
        word5_idx = 0;
        for (auto word5 : nodes5Ref.mask) {
            const auto& base_bit_4_idx = ((uint32_t)word5_idx++) * 64;
            for (; word5 != 0; word5 &= word5 - 1) {
                const auto& bit_4_index = base_bit_4_idx + (uint32_t)count_trailing_zeros(word5);
                const auto& nodes4Ref = nodes5Ref.nodes.at(bit_4_index);
                size_t word4_idx = 0;
                for (auto word4 : nodes4Ref.mask) {
                    const auto& base_bit_3_idx = ((uint32_t)word4_idx++) * 64;
                    for (; word4 != 0; word4 &= word4 - 1) {
                        const auto& bit_3_index = base_bit_3_idx + (uint32_t)count_trailing_zeros(word4);
                        const auto& nodes3Ref = nodes4Ref.nodes.at(bit_3_index);
                        write_data_arr<uint64_t>(fp, nodes3Ref.mask, 8);
                        write_data<uint8_t>(fp, 6);
                        write_data_arr<std::array<TType, TCount>>(fp, nodes3Ref.data, 512);
                    }
                }
            }
        }
    }

    void writeMetadata(FILE* fp) {
        // Number of entries
        write_data<uint32_t>(fp, 5);
        write_meta_string(fp, "class", "unknown");
        write_meta_string(fp, "file_compression", "none");
        write_meta_vec3i(fp, "file_bbox_max", {(int32_t)m_Volume.upperBound.x, (int32_t)m_Volume.upperBound.y, (int32_t)m_Volume.upperBound.z});
        write_meta_vec3i(fp, "file_bbox_min", {(int32_t)m_Volume.lowerBound.x, (int32_t)m_Volume.lowerBound.y, (int32_t)m_Volume.lowerBound.z});
        write_meta_string(fp, "name", m_Name);
    }

    void writeTransform(FILE* fp) {
        write_name(fp, "UniformScaleTranslateMap");
        // write Translation
        const auto& center = m_Volume.GetCenter();
        write_data<double>(fp, -center.x);
        write_data<double>(fp, -center.y);
        write_data<double>(fp, -center.z);
        // write ScaleValues
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        // write VoxelSize
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        // write ScaleValuesInverse
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        // write InvScaleSqr
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        write_data<double>(fp, 1.0);
        // write InvTwiceScale;
        write_data<double>(fp, 0.5);
        write_data<double>(fp, 0.5);
        write_data<double>(fp, 0.5);
    }

    // thoses functions must be specialized or derived
    virtual std::string getTypeName() {
        std::cout << "getTypeName is not specialized for your Type" << std::endl;
        assert(0);
        return "";
    }

private:
    Node5 m_Nodes;

public:
    VdbTree(const std::string& vName) : ATree(vName) {
    }
    virtual ~VdbTree() = default;

    bool addVoxel(uint32_t vX, uint32_t vY, uint32_t vZ, void* vDatas, size_t vByteSize, size_t vCount) override final {
        if (vDatas != nullptr && vByteSize == sizeof(TType) && vCount == TCount) {
            m_Volume.Combine(ez::dvec3((float)vX, (float)vY, (float)vZ));
            const auto& bit_index_4 = getBitIndex4(vX, vY, vZ);
            const auto& bit_index_3 = getBitIndex3(vX, vY, vZ);
            const auto& bit_index_0 = getBitIndex0(vX, vY, vZ);
            auto& nodes4Ref = m_Nodes.nodes[bit_index_4];
            auto& nodes3Ref = nodes4Ref.nodes[bit_index_3];
            m_Nodes.mask[bit_index_4 >> 6] |= static_cast<uint64_t>(1) << (bit_index_4 & (64 - 1));    // active the voxel 4
            nodes4Ref.mask[bit_index_3 >> 6] |= static_cast<uint64_t>(1) << (bit_index_3 & (64 - 1));  // active the voxel 3
            nodes3Ref.mask[bit_index_0 >> 6] |= static_cast<uint64_t>(1) << (bit_index_0 & (64 - 1));  // active the voxel 0
            memcpy(&nodes3Ref.data[bit_index_0], vDatas, vByteSize * vCount);
            return true;
        }
        return false;
    }

    bool addVoxel(uint32_t vX, uint32_t vY, uint32_t vZ, const std::array<TType, TCount>& vDatas) {
        return addVoxel(vX, vY, vZ, const_cast<TType*>(vDatas.data()), sizeof(TType), TCount);
    }

    void write(FILE* fp) override {
        write_name(fp, m_Name);
        write_name(fp, getTypeName());
        write_data<uint32_t>(fp, 0);  // Instance parent
        uint64_t stream_pos = ftell(fp);
        write_data<uint64_t>(fp, stream_pos + sizeof(uint64_t) * 3);  // grid pos
        write_data<uint64_t>(fp, 0);                                  // block pos
        write_data<uint64_t>(fp, 0);                                  // end pos
        write_data<uint32_t>(fp, 0);                                  // compression
        writeMetadata(fp);
        writeTransform(fp);
        writeTree(fp);
    }
};

template <typename TType>
class VdbScalarGrid : public VdbTree<TType, 1> {
protected:
    std::string getTypeName() override {
        std::cout << "getTypeName is not specialized for your Type" << std::endl;
        assert(0);
        return "";
    }

public:
    VdbScalarGrid(const std::string& vName) : VdbTree<TType, 1>(vName) {
    }
    virtual ~VdbScalarGrid() = default;
    bool addVoxel(uint32_t vX, uint32_t vY, uint32_t vZ, const TType& vDatas) {
        return VdbTree<TType, 1>::addVoxel(vX, vY, vZ, (void*)&vDatas, sizeof(TType), 1U);
    }
};

typedef VdbScalarGrid<float> VdbFloatGrid;
template <>
inline std::string VdbFloatGrid::getTypeName() {
    return "Tree_float_5_4_3";
}

typedef VdbScalarGrid<double> VdbDoubleGrid;
template <>
inline std::string VdbDoubleGrid::getTypeName() {
    return "Tree_double_5_4_3";
}

template <typename TType>
class VdbVec3Grid : public VdbTree<TType, 3> {
protected:
    std::string getTypeName() override {
        std::cout << "getTypeName is not specialized for your Type" << std::endl;
        assert(0);
        return "";
    }

public:
    VdbVec3Grid(const std::string& vName) : VdbTree<TType, 3>(vName) {
    }
    virtual ~VdbVec3Grid() = default;
    bool addVoxel(uint32_t vX, uint32_t vY, uint32_t vZ, const TType& v0, const TType& v1, const TType& v2) {
        return VdbTree<TType, 3>::addVoxel(vX, vY, vZ, {v0, v1, v2});
    }
};

typedef VdbVec3Grid<float> VdbVec3sGrid;
template <>
inline std::string VdbVec3sGrid::getTypeName() {
    return "Tree_vec3s_5_4_3";
}

typedef VdbVec3Grid<double> VdbVec3dGrid;
template <>
inline std::string VdbVec3dGrid::getTypeName() {
    return "Tree_vec3d_5_4_3";
}

typedef VdbVec3Grid<int32_t> VdbVec3iGrid;
template <>
inline std::string VdbVec3iGrid::getTypeName() {
    return "Tree_vec3i_5_4_3";
}

typedef VdbVec3Grid<uint32_t> VdbVec3uiGrid;
template <>
inline std::string VdbVec3uiGrid::getTypeName() {
    return "Tree_vec3ui_5_4_3";
}

typedef uint32_t KeyFrame;
typedef uint32_t LayerId;

class Writer {
private:
    typedef std::unordered_map<LayerId, std::unique_ptr<ATree>> LayerContainer;
    std::unordered_map<KeyFrame, LayerContainer> m_Trees;
    KeyFrame m_CurrentKeyFrame = 0U;
    FILE* m_File = nullptr;
    int32_t m_LastError = 0;

public:
    template <typename TTtree>
    TTtree& getLayer(uint32_t vLayerId, const std::string& vLayerName) {
        auto& key = m_Trees[m_CurrentKeyFrame];
        if (key.find(vLayerId) == key.end()) {
            key[vLayerId] = std::unique_ptr<TTtree>(new TTtree(vLayerName));
        }
        return static_cast<TTtree&>(*(key.at(vLayerId).get()));
    }

    Writer& setKeyFrame(uint32_t vKeyFrame) {
        m_CurrentKeyFrame = vKeyFrame;
        return *this;
    }

    Writer& save(const std::string& vFilePathName) {
        if (!vFilePathName.empty()) {
            auto dot_p = vFilePathName.find_last_of('.');
            if (dot_p != std::string::npos) {
                auto base_file_path_name = vFilePathName.substr(0, dot_p);
                size_t idx = 1;
                for (auto& vdb : m_Trees) {
                    std::stringstream str;
                    if (m_Trees.size() > 1) {
                        str << base_file_path_name << "_" << std::setfill('0') << std::setw(4) << idx++ << ".vdb";  // many frames
                    } else {
                        str << base_file_path_name << ".vdb";
                    }
                    if (openFileForWriting(str.str())) {
                        writeVdb(m_File, vdb.second);
                        closeFile();
                    } else {
                        std::cout << "Error, cant write to the file " << str.str() << std::endl;
                    }
                }
            }
        }
        return *this;
    }

    // common grid types
    VdbFloatGrid& getFloatLayer(uint32_t vLayerId, const std::string& vLayerName) {
        return getLayer<VdbFloatGrid>(vLayerId, vLayerName);
    }
    VdbDoubleGrid& getDoubleLayer(uint32_t vLayerId, const std::string& vLayerName) {
        return getLayer<VdbDoubleGrid>(vLayerId, vLayerName);
    }
    VdbVec3sGrid& getVec3sLayer(uint32_t vLayerId, const std::string& vLayerName) {
        return getLayer<VdbVec3sGrid>(vLayerId, vLayerName);
    }
    VdbVec3dGrid& getVec3dLayer(uint32_t vLayerId, const std::string& vLayerName) {
        return getLayer<VdbVec3dGrid>(vLayerId, vLayerName);
    }
    VdbVec3iGrid& getVec3iLayer(uint32_t vLayerId, const std::string& vLayerName) {
        return getLayer<VdbVec3iGrid>(vLayerId, vLayerName);
    }
    VdbVec3uiGrid& getVec3uiLayer(uint32_t vLayerId, const std::string& vLayerName) {
        return getLayer<VdbVec3uiGrid>(vLayerId, vLayerName);
    }

private:
    static void writeVdb(FILE* fp, const LayerContainer& vTrees) {
        std::array<uint8_t, 8> header = {0x20, 0x42, 0x44, 0x56, 0x0, 0x0, 0x0, 0x0};
        write_ptr(fp, header.data(), sizeof(uint8_t), header.size());
        write_data<uint32_t>(fp, 224);
        write_data<uint32_t>(fp, 8);  // major openvdb 8
        write_data<uint32_t>(fp, 1);  // minor openvdb 8.1
        write_data<uint8_t>(fp, 0);
        write_string(fp, "00000000-0000-0000-0000-000000000000");
        write_data<uint32_t>(fp, 0);
        write_data<uint32_t>(fp, (uint32_t)vTrees.size());
        for (auto& tree : vTrees) {
            tree.second->write(fp);
        }
    }

    bool openFileForWriting(const std::string& vFilePathName) {
#if _MSC_VER
        m_LastError = fopen_s(&m_File, vFilePathName.c_str(), "wb");
#else
        m_File = fopen(vFilePathName.c_str(), "wb");
        m_LastError = m_File ? 0 : errno;
#endif
        return (m_LastError == 0);
    }

    void closeFile() {
        fclose(m_File);
    }

    long getFilePos() const {
        return ftell(m_File);
    }

    void setFilePos(const long& vPos) {
        fseek(m_File, vPos, SEEK_SET);
    }
};

}  // namespace vdb
}  // namespace file
}  // namespace ez
