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

// EzGraph is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <set>
#include <type_traits>
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <functional>

namespace ez {
/////////////////////////////////////
///// Utils /////////////////////////
/////////////////////////////////////

namespace Utils {
// if the shared_ptr exit in the container return the iterator
template <typename T>
typename std::vector<std::shared_ptr<T>>::iterator  //
isSharedPtrExistInVector(const std::shared_ptr<T> &vPtr, std::vector<std::shared_ptr<T>> &vContainer) {
    auto ret = vContainer.end();
    if (vPtr != nullptr) {
        ret = vContainer.begin();
        for (; ret != vContainer.end(); ++ret) {
            if (*ret == vPtr) {
                break;
            }
        }
    }
    return ret;
}

// if the weak_ptr exit in the container return the iterator
template <typename T>
typename std::vector<std::weak_ptr<T>>::iterator  //
isWeakPtrExistInVector(const std::weak_ptr<T> &vWeak, std::vector<std::weak_ptr<T>> &vContainer) {
    auto ret = vContainer.end();
    if (!vWeak.expired()) {
        auto ptr = vWeak.lock();
        ret = vContainer.begin();
        for (; ret != vContainer.end(); ++ret) {
            if (ret->lock() == ptr) {
                break;
            }
        }
    }
    return ret;
}
}  // namespace Utils

/////////////////////////////////////
///// DEFS //////////////////////////
/////////////////////////////////////

enum class SlotDir { INPUT = 0, OUTPUT, Count };
enum class RetCodes {
    SUCCESS = 0,
    FAILED,
    FAILED_SLOT_PTR_NULL,
    FAILED_SLOT_ALREADY_EXIST,
    FAILED_SLOT_NOT_FOUND,
    FAILED_NODE_PTR_NULL,
    FAILED_NODE_ALREADY_EXIST,
    FAILED_NODE_NOT_FOUND,
    Count
};

class Slot;
typedef std::shared_ptr<Slot> SlotPtr;
typedef std::weak_ptr<Slot> SlotWeak;

class Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::weak_ptr<Node> NodeWeak;

class Graph;
typedef std::shared_ptr<Graph> GraphPtr;
typedef std::weak_ptr<Graph> GraphWeak;

typedef uintptr_t Uuid;

typedef void *UserDatas;

/////////////////////////////////////
///// UUID //////////////////////////
/////////////////////////////////////

class UUID {
    Uuid m_Uuid = 0U;

public:
    explicit UUID(void *vPtr) { m_Uuid = reinterpret_cast<Uuid>(vPtr); }
    virtual ~UUID() = default;
    Uuid getUuid() const { return m_Uuid; }
    void setUuid(const Uuid vUUID) { m_Uuid = vUUID; }
};

/////////////////////////////////////
///// SLOT //////////////////////////
/////////////////////////////////////

struct SlotDatas {
    std::string name;
    std::string type;
    UserDatas userDatas = nullptr;
    SlotDir dir = SlotDir::INPUT;
    SlotDatas() = default;
    SlotDatas(std::string vName, std::string vType, const SlotDir vSlotDir, UserDatas vUserDatas = nullptr)
        : name(std::move(vName)), type(std::move(vType)), userDatas(vUserDatas), dir(vSlotDir) {}
};

struct EvalDatas {
    size_t frame = 0U;
};

class Slot : public UUID {
    friend class Node;
    friend class Graph;

protected:
    SlotWeak m_This;
    NodeWeak m_ParentNode;
    std::shared_ptr<SlotDatas> mp_SlotDatas;
    std::vector<SlotWeak> m_ConnectedSlots;
    EvalDatas m_LastEvaluatedDatas;

public:
    Slot() : UUID(this) {}
    template <typename T, typename = std::enable_if<std::is_base_of<SlotDatas, T>::value>>
    explicit Slot(const T &vDatas) : UUID(this), mp_SlotDatas(std::make_shared<T>(vDatas)) {}
    ~Slot() override {
        mp_SlotDatas.reset();
        m_ConnectedSlots.clear();
        unit();
    }

    virtual bool init() { return true; }
    virtual void unit() {}

    // enable_if remove the need to use a slow dynamic_cast
    template <typename T, typename = std::enable_if<std::is_base_of<SlotDatas, T>::value>>
    const T &getDatas() const {
        return static_cast<const T &>(*mp_SlotDatas);
    }

    // enable_if remove the need to use a slow dynamic_cast
    template <typename T, typename = std::enable_if<std::is_base_of<SlotDatas, T>::value>>
    T &getDatasRef() {
        return static_cast<T &>(*mp_SlotDatas);
    }

    void setParentNode(NodeWeak vNodeWeak) { m_ParentNode = std::move(vNodeWeak); }
    NodeWeak getParentNode() { return m_ParentNode; }
    const std::vector<SlotWeak> &m_getConnectedSlots() { return m_ConnectedSlots; }
    void setLastEvaluatedDatas(const EvalDatas vUserDatas) { m_LastEvaluatedDatas = vUserDatas; }
    const EvalDatas &getLastEvaluatedDatas() const { return m_LastEvaluatedDatas; }

protected:
    void m_setThis(const SlotWeak &vThis) { m_This = vThis; }

    template <typename T, typename = std::enable_if<std::is_base_of<SlotDatas, T>::value>>
    explicit Slot(std::shared_ptr<T> vpDatas) : UUID(this), mp_SlotDatas(std::move(vpDatas)) {}

    RetCodes m_connectSlot(const SlotWeak &vSlot) {
        auto ret = RetCodes::FAILED_SLOT_PTR_NULL;
        if (!vSlot.expired()) {
            m_ConnectedSlots.push_back(vSlot);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    RetCodes m_disconnectSlot(const SlotWeak &vSlot) {
        auto ret = RetCodes::FAILED_SLOT_ALREADY_EXIST;
        const auto it = Utils::isWeakPtrExistInVector(vSlot, m_ConnectedSlots);
        if (it != m_ConnectedSlots.end()) {
            m_ConnectedSlots.erase(it);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    void m_disconnect() { m_ConnectedSlots.clear(); }
};

/////////////////////////////////////
///// NODE //////////////////////////
/////////////////////////////////////

struct NodeDatas {
    std::string name;
    std::string type;
    UserDatas userDatas = nullptr;
    NodeDatas() = default;
    NodeDatas(std::string vName, std::string vType, UserDatas vUserDatas = nullptr)
        : name(std::move(vName)), type(std::move(vType)), userDatas(vUserDatas) {}
};

class Node : public UUID {
    friend class Graph;
    NodeWeak m_This;
    GraphWeak m_ParentGraph;
    bool dirty = false;
    std::shared_ptr<NodeDatas> mp_NodeDatas;
    std::vector<SlotPtr> m_Inputs;
    std::vector<SlotPtr> m_Outputs;

public:
    Node() : UUID(this) {}
    template <typename T, typename = std::enable_if<std::is_base_of<NodeDatas, T>::value>>
    explicit Node(const T &vDatas) : UUID(this), mp_NodeDatas(std::make_shared<T>(vDatas)) {}
    ~Node() override {
        m_This.reset();
        m_ParentGraph.reset();
        mp_NodeDatas.reset();
        m_Inputs.clear();
        m_Outputs.clear();
        unit();
    }

    virtual bool init() { return true; }
    virtual void unit() {}

    // Datas
    void setParentGraph(const GraphWeak &vParentGraph) { m_ParentGraph = vParentGraph; }
    GraphWeak getParentGraph() { return m_ParentGraph; }

    // enable_if remove the need to use a slow dynamic_cast
    template <typename T, typename = std::enable_if<std::is_base_of<NodeDatas, T>::value>>
    const T &getDatas() const {
        return static_cast<const T &>(*mp_NodeDatas);
    }

    // enable_if remove the need to use a slow dynamic_cast
    template <typename T, typename = std::enable_if<std::is_base_of<NodeDatas, T>::value>>
    T &getDatasRef() {
        return static_cast<T &>(*mp_NodeDatas);
    }
    void setDirty(const bool vFlag) { dirty = vFlag; }
    bool isDirty() const { return dirty; }

protected:  // Node
    NodeWeak m_getThis() { return m_This; }
    void m_setThis(const NodeWeak &vThis) { m_This = vThis; }

    void m_setSlotThis(SlotPtr vSlotPtr) {
        if (vSlotPtr != nullptr) {
            vSlotPtr->m_setThis(vSlotPtr);
        }
    }

    template <typename T, typename = std::enable_if<std::is_base_of<NodeDatas, T>::value>>
    explicit Node(std::shared_ptr<T> vpDatas) : UUID(this), mp_NodeDatas(std::move(vpDatas)) {}

    // Slots
    RetCodes m_addSlot(const SlotPtr &vSlotPtr) {
        auto ret = RetCodes::FAILED;
        if (vSlotPtr != nullptr) {
            vSlotPtr->m_setThis(vSlotPtr);
            const auto &datas = vSlotPtr->getDatas<SlotDatas>();
            if (datas.dir == SlotDir::INPUT) {
                const auto it = Utils::isSharedPtrExistInVector(vSlotPtr, m_Inputs);
                if (it == m_Inputs.end()) {
                    vSlotPtr->setParentNode(m_This);
                    m_Inputs.push_back(vSlotPtr);
                    ret = RetCodes::SUCCESS;
                } else {
                    ret = RetCodes::FAILED_SLOT_ALREADY_EXIST;
                }
            } else if (datas.dir == SlotDir::OUTPUT) {
                const auto it = Utils::isSharedPtrExistInVector(vSlotPtr, m_Outputs);
                if (it == m_Outputs.end()) {
                    vSlotPtr->setParentNode(m_This);
                    m_Outputs.push_back(vSlotPtr);
                    ret = RetCodes::SUCCESS;
                } else {
                    ret = RetCodes::FAILED_SLOT_ALREADY_EXIST;
                }
            }
        }
        return ret;
    }

    template <typename T, typename = std::enable_if<std::is_base_of<NodeDatas, T>::value>>
    SlotPtr m_addSlot(const SlotDatas &vSlotDatas, RetCodes *vOutRetCodes) {
        if (vOutRetCodes != nullptr) {
            *vOutRetCodes = RetCodes::FAILED_NODE_PTR_NULL;
        }
        auto slot_ptr = std::make_shared<Slot>(vSlotDatas);
        const auto ret_code = m_addSlot(slot_ptr);
        if (vOutRetCodes != nullptr) {
            *vOutRetCodes = ret_code;
        }
        return slot_ptr;
    }

    RetCodes m_delSlot(const SlotPtr &vSlotPtr) {
        auto ret = m_delInputSlot(vSlotPtr);
        if (ret != RetCodes::SUCCESS) {
            ret = m_delOutputSlot(vSlotPtr);
        }
        return ret;
    }

    RetCodes m_delInputSlot(const SlotPtr &vSlotPtr) {
        auto ret = RetCodes::FAILED_SLOT_NOT_FOUND;
        const auto it = Utils::isSharedPtrExistInVector(vSlotPtr, m_Inputs);
        if (it != m_Inputs.end()) {
            m_Inputs.erase(it);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    RetCodes m_delOutputSlot(const SlotPtr &vSlotPtr) {
        auto ret = RetCodes::FAILED_SLOT_NOT_FOUND;
        const auto it = Utils::isSharedPtrExistInVector(vSlotPtr, m_Outputs);
        if (it != m_Outputs.end()) {
            m_Outputs.erase(it);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    const std::vector<SlotPtr> &m_getInputSlots() { return m_Inputs; }
    std::vector<SlotPtr> &m_getInputSlotsRef() { return m_Inputs; }

    const std::vector<SlotPtr> &m_getOutputSlots() { return m_Outputs; }
    std::vector<SlotPtr> &m_getOutputSlotsRef() { return m_Outputs; }
};

/////////////////////////////////////
///// GRAPH /////////////////////////
/////////////////////////////////////

struct GraphDatas {
    std::string name;
    std::string type;
    UserDatas userDatas = nullptr;
    GraphDatas() = default;
    GraphDatas(std::string vName, std::string vType, UserDatas vUserDatas = nullptr)
        : name(std::move(vName)), type(std::move(vType)), userDatas(vUserDatas) {}
};

class Graph : public UUID {
    GraphWeak m_This;
    GraphWeak m_ParentGraph;
    NodeWeak m_ParentNode;
    bool dirty = false;
    std::shared_ptr<GraphDatas> mp_GraphDatas;
    std::vector<NodePtr> m_Nodes;

public:
    Graph() : UUID(this) {}
    template <typename T, typename = std::enable_if<std::is_base_of<GraphDatas, T>::value>>
    explicit Graph(const T &vDatas) : UUID(this), mp_GraphDatas(std::make_shared<T>(vDatas)) {}
    ~Graph() override {
        m_This.reset();
        m_ParentNode.reset();
        mp_GraphDatas.reset();
        m_Nodes.clear();
        unit();
    }

    virtual bool init() { return true; }
    virtual void unit() {}

    // Datas
    void setParentNode(const NodeWeak &vParentNode) { m_ParentNode = vParentNode; }
    NodeWeak getParentNode() { return m_ParentNode; }
    void setParentGraph(const GraphWeak &vParentNode) { m_ParentGraph = vParentNode; }
    GraphWeak getParentGraph() { return m_ParentGraph; }

    // enable_if remove the need to use a slow dynamic_cast
    template <typename T, typename = std::enable_if<std::is_base_of<GraphDatas, T>::value>>
    const T &getDatas() const {
        return static_cast<const T &>(*mp_GraphDatas);
    }

    // enable_if remove the need to use a slow dynamic_cast
    template <typename T, typename = std::enable_if<std::is_base_of<GraphDatas, T>::value>>
    T &getDatasRef() {
        return static_cast<T &>(*mp_GraphDatas);
    }

    const std::vector<NodePtr> &getNodes() const { return m_Nodes; }
    std::vector<NodePtr> &getNodesRef() { return m_Nodes; }

    void setDirty(const bool vFlag) { dirty = vFlag; }
    bool isDirty() const { return dirty; }

protected:  // Node
    GraphWeak m_getThis() { return m_This; }
    void m_setThis(const GraphWeak &vThis) { m_This = vThis; }

    void m_setNodeThis(NodePtr vNodePtr) {
        if (vNodePtr != nullptr) {
            vNodePtr->m_setThis(vNodePtr);
        }
    }

    template <typename T, typename = std::enable_if<std::is_base_of<GraphDatas, T>::value>>
    explicit Graph(std::shared_ptr<T> vpDatas) : UUID(this), mp_GraphDatas(std::move(vpDatas)) {}

    RetCodes m_addNode(const NodePtr &vNodePtr) {
        auto ret = RetCodes::FAILED_NODE_PTR_NULL;
        if (vNodePtr != nullptr) {
            vNodePtr->m_setThis(vNodePtr);
            vNodePtr->setParentGraph(m_This);
            m_Nodes.push_back(vNodePtr);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    RetCodes m_delNode(const NodePtr &vNodePtr) {
        auto ret = RetCodes::FAILED_NODE_NOT_FOUND;
        const auto it = Utils::isSharedPtrExistInVector(vNodePtr, m_Nodes);
        if (it != m_Nodes.end()) {
            m_Nodes.erase(it);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    static RetCodes m_connectSlots(const SlotWeak &vFrom, const SlotWeak &vTo) {
        auto ret = RetCodes::FAILED_SLOT_PTR_NULL;
        if (!vFrom.expired() && !vTo.expired()) {
            const auto fromPtr = vFrom.lock();
            const auto toPtr = vTo.lock();
            if (fromPtr != nullptr && toPtr != nullptr) {
                ret = fromPtr->m_connectSlot(vTo);
                if (ret == RetCodes::SUCCESS) {
                    ret = toPtr->m_connectSlot(vFrom);
                    if (ret != RetCodes::SUCCESS) {
                        fromPtr->m_connectSlot(vTo);
                    }
                }
            }
        }
        return ret;
    }

    static RetCodes m_disconnectSlots(const SlotWeak &vFrom, const SlotWeak &vTo) {
        auto ret = RetCodes::FAILED_SLOT_PTR_NULL;
        if (!vFrom.expired() && !vTo.expired()) {
            const auto fromPtr = vFrom.lock();
            const auto toPtr = vTo.lock();
            if (fromPtr != nullptr && toPtr != nullptr) {
                /*ret =*/
                fromPtr->m_disconnectSlot(vTo);
                // even if the last is in error, we try the diconnect
                ret = toPtr->m_disconnectSlot(vFrom);
            }
        }
        return ret;
    }
};

}  // namespace ez
