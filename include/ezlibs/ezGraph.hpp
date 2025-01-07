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
    template <typename T = SlotDatas>
    explicit Slot(const T &vDatas) : UUID(this), mp_SlotDatas(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<SlotDatas, T>::value, "T must derive of ez::SlotDatas");
    }
    ~Slot() override { unit(); }

    virtual bool init() {
        assert(!m_This.expired() && "m_This msut be defined with m_setThis suring the ceration");
        return true;
    }
    virtual void unit() {
        // we must rsdet slots
        m_ConnectedSlots.clear();
        // befaore datas
        mp_SlotDatas.reset();
    }

    template <typename T = SlotDatas>
    const T &getDatas() const {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<SlotDatas, T>::value, "T must derive of ez::SlotDatas");
        return static_cast<const T &>(*mp_SlotDatas);
    }

    template <typename T = SlotDatas>
    T &getDatasRef() {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<SlotDatas, T>::value, "T must derive of ez::SlotDatas");
        return static_cast<T &>(*mp_SlotDatas);
    }

    void setParentNode(NodeWeak vNodeWeak) { m_ParentNode = std::move(vNodeWeak); }
    template <typename T = Node>
    std::weak_ptr<T> getParentNode() {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<Node, T>::value, "T must derive of ez::Node");
        return std::static_pointer_cast<T>(m_ParentNode.lock());
    }
    const std::vector<SlotWeak> &m_getConnectedSlots() { return m_ConnectedSlots; }
    void setLastEvaluatedDatas(const EvalDatas vUserDatas) { m_LastEvaluatedDatas = vUserDatas; }
    const EvalDatas &getLastEvaluatedDatas() const { return m_LastEvaluatedDatas; }

protected:
    template <typename T = Slot>
    std::weak_ptr<T> m_getThis() {
        static_assert(std::is_base_of<Slot, T>::value, "T must derive of ez::Slot");
        assert(!m_This.expired() && "m_This msut be defined with m_setThis suring the ceration");
        return std::static_pointer_cast<T>(m_This.lock());
    }
    void m_setThis(const SlotWeak &vThis) { m_This = vThis; }

    template <typename T = SlotDatas>
    explicit Slot(std::shared_ptr<T> vpDatas) : UUID(this), mp_SlotDatas(std::move(vpDatas)) {
        static_assert(std::is_base_of<SlotDatas, T>::value, "T must derive of ez::SlotDatas");
    }

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
    NodeDatas(std::string vName, std::string vType, UserDatas vUserDatas = nullptr) : name(std::move(vName)), type(std::move(vType)), userDatas(vUserDatas) {}
};

class Node : public UUID {
    friend class Graph;
    NodeWeak m_This;
    GraphWeak m_ParentGraph;
    bool dirty = false;
    std::shared_ptr<NodeDatas> mp_NodeDatas;
    std::vector<SlotPtr> m_Inputs;
    std::vector<SlotWeak> m_InputWeaks;
    std::vector<SlotPtr> m_Outputs;
    std::vector<SlotWeak> m_OutputWeaks;

public:
    Node() : UUID(this) {}
    template <typename T = NodeDatas>
    explicit Node(const T &vDatas) : UUID(this), mp_NodeDatas(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::NodeDatas");
    }
    ~Node() override { unit(); }

    virtual bool init() {
        assert(!m_This.expired() && "m_This msut be defined with m_setThis suring the ceration");
        return true;
    }
    virtual void unit() {
        // we must reset slots 
        m_Inputs.clear();
        m_Outputs.clear();
        // before reset this ans parentGraph
        m_This.reset();
        m_ParentGraph.reset();
        mp_NodeDatas.reset();
    }

    // Datas
    void setParentGraph(const GraphWeak &vParentGraph) { m_ParentGraph = vParentGraph; }
    template <typename T = Graph>
    std::weak_ptr<T> getParentGraph() {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<Graph, T>::value, "T must derive of ez::Graph");
        return std::static_pointer_cast<T>(m_ParentGraph.lock());
    }

    template <typename T = NodeDatas>
    const T &getDatas() const {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::NodeDatas");
        return static_cast<const T &>(*mp_NodeDatas);
    }

    template <typename T = NodeDatas>
    T &getDatasRef() {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::NodeDatas");
        return static_cast<T &>(*mp_NodeDatas);
    }
    void setDirty(const bool vFlag) { dirty = vFlag; }
    bool isDirty() const { return dirty; }

protected:  // Node
    template <typename T = Node>
    std::weak_ptr<T> m_getThis() {
        static_assert(std::is_base_of<Node, T>::value, "T must derive of ez::Node");
        assert(!m_This.expired() && "m_This msut be defined with m_setThis suring the ceration");
        return std::static_pointer_cast<T>(m_This.lock());
    }
    void m_setThis(const NodeWeak &vThis) { m_This = vThis; }

    void m_setSlotThis(SlotPtr vSlotPtr) {
        if (vSlotPtr != nullptr) {
            vSlotPtr->m_setThis(vSlotPtr);
        }
    }

    template <typename T = NodeDatas>
    explicit Node(std::shared_ptr<T> vpDatas) : UUID(this), mp_NodeDatas(std::move(vpDatas)) {
        static_assert(std::is_base_of<NodeDatas, T>::value, "T must derive of ez::NodeDatas");
    }

    // Slots
    RetCodes m_addSlot(const SlotPtr &vSlotPtr) {
        auto ret = RetCodes::FAILED;
        if (vSlotPtr != nullptr) {
            vSlotPtr->m_setThis(vSlotPtr);
            const auto &datas = vSlotPtr->getDatas<SlotDatas>();
            if (datas.dir == SlotDir::INPUT) {
                const auto it = Utils::isSharedPtrExistInVector(vSlotPtr, m_Inputs);
                if (it == m_Inputs.end()) {
                    vSlotPtr->setParentNode(m_getThis());
                    m_Inputs.push_back(vSlotPtr);
                    m_InputWeaks.push_back(vSlotPtr);
                    ret = RetCodes::SUCCESS;
                } else {
                    ret = RetCodes::FAILED_SLOT_ALREADY_EXIST;
                }
            } else if (datas.dir == SlotDir::OUTPUT) {
                const auto it = Utils::isSharedPtrExistInVector(vSlotPtr, m_Outputs);
                if (it == m_Outputs.end()) {
                    vSlotPtr->setParentNode(m_getThis());
                    m_Outputs.push_back(vSlotPtr);
                    m_OutputWeaks.push_back(vSlotPtr);
                    ret = RetCodes::SUCCESS;
                } else {
                    ret = RetCodes::FAILED_SLOT_ALREADY_EXIST;
                }
            }
        }
        return ret;
    }

    template <typename T = Slot>
    std::shared_ptr<T> m_addSlot(const SlotDatas &vSlotDatas, RetCodes *vOutRetCodes) {
        static_assert(std::is_base_of<Slot, T>::value, "T must derive of ez::Slot");
        if (vOutRetCodes != nullptr) {
            *vOutRetCodes = RetCodes::FAILED_NODE_PTR_NULL;
        }
        auto slot_ptr = std::make_shared<T>(vSlotDatas);
        const auto ret_code = m_addSlot(slot_ptr);
        if (vOutRetCodes != nullptr) {
            *vOutRetCodes = ret_code;
        }
        return slot_ptr;
    }

    RetCodes m_delSlot(const SlotWeak &vSlot) {
        auto ret = m_delInputSlot(vSlot);
        if (ret != RetCodes::SUCCESS) {
            ret = m_delOutputSlot(vSlot);
        }
        return ret;
    }

    RetCodes m_delInputSlot(const SlotWeak &vSlot) {
        auto ret = RetCodes::FAILED_SLOT_NOT_FOUND;
        // we must detroy the weak before the related shared ptr
        auto itWeak = Utils::isWeakPtrExistInVector(vSlot, m_InputWeaks);
        if (itWeak != m_InputWeaks.end()) {
            m_InputWeaks.erase(itWeak);
            // so next, the shared ptr
            auto itShared = Utils::isSharedPtrExistInVector(vSlot.lock(), m_Inputs);
            if (itShared != m_Inputs.end()) {
                itShared->get()->unit();
                m_Inputs.erase(itShared);
                ret = RetCodes::SUCCESS;
            }
        }
        return ret;
    }

    RetCodes m_delOutputSlot(const SlotWeak &vSlot) {
        auto ret = RetCodes::FAILED_SLOT_NOT_FOUND;
        // we must detroy the weak before the related shared ptr
        auto itWeak = Utils::isWeakPtrExistInVector(vSlot, m_OutputWeaks);
        if (itWeak != m_OutputWeaks.end()) {
            m_OutputWeaks.erase(itWeak);
            // so next, the shared ptr
            const auto itShared = Utils::isSharedPtrExistInVector(vSlot.lock(), m_Outputs);
            if (itShared != m_Outputs.end()) {
                itShared->get()->unit();
                m_Outputs.erase(itShared);
                ret = RetCodes::SUCCESS;
            }
        }
        return ret;
    }

    const std::vector<SlotWeak> &m_getInputSlots() { return m_InputWeaks; }
    std::vector<SlotWeak> &m_getInputSlotsRef() { return m_InputWeaks; }

    const std::vector<SlotWeak> &m_getOutputSlots() { return m_OutputWeaks; }
    std::vector<SlotWeak> &m_getOutputSlotsRef() { return m_OutputWeaks; }
};

/////////////////////////////////////
///// GRAPH /////////////////////////
/////////////////////////////////////

struct GraphDatas {
    std::string name;
    std::string type;
    UserDatas userDatas = nullptr;
    GraphDatas() = default;
    GraphDatas(std::string vName, std::string vType, UserDatas vUserDatas = nullptr) : name(std::move(vName)), type(std::move(vType)), userDatas(vUserDatas) {}
};

class Graph : public UUID {
    GraphWeak m_This;
    NodeWeak m_ParentNode;
    bool dirty = false;
    std::shared_ptr<GraphDatas> mp_GraphDatas;
    std::vector<NodePtr> m_Nodes;
    std::vector<NodeWeak> m_NodeWeaks;

public:
    Graph() : UUID(this) {}
    template <typename T = GraphDatas>
    explicit Graph(const T &vDatas) : UUID(this), mp_GraphDatas(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<GraphDatas, T>::value, "T must derive of ez::GraphDatas");
    }
    ~Graph() override { unit(); }

    virtual bool init() {
        assert(!m_This.expired() && "m_This msut be defined with m_setThis suring the ceration");
        return true;
    }
    virtual void unit() {
        m_This.reset();
        m_ParentNode.reset();
        mp_GraphDatas.reset();
        m_Nodes.clear();
    }

    // Datas
    void setParentNode(const NodeWeak &vParentNode) { m_ParentNode = vParentNode; }
    template <typename T = Node>
    std::weak_ptr<T> getParentNode() {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<Node, T>::value, "T must derive of ez::Node");
        return std::static_pointer_cast<T>(m_ParentNode.lock());
    }

    template <typename T = GraphDatas>
    const T &getDatas() const {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<GraphDatas, T>::value, "T must derive of ez::GraphDatas");
        return static_cast<const T &>(*mp_GraphDatas);
    }

    template <typename T = GraphDatas>
    T &getDatasRef() {
        // remove the need to use a slow dynamic_cast
        static_assert(std::is_base_of<GraphDatas, T>::value, "T must derive of ez::GraphDatas");
        return static_cast<T &>(*mp_GraphDatas);
    }

    const std::vector<NodeWeak> &getNodes() const { return m_NodeWeaks; }
    std::vector<NodeWeak> &getNodesRef() { return m_NodeWeaks; }

    void setDirty(const bool vFlag) { dirty = vFlag; }
    bool isDirty() const { return dirty; }

protected:  // Node
    template <typename T = Graph>
    std::weak_ptr<T> m_getThis() {
        static_assert(std::is_base_of<Graph, T>::value, "T must derive of ez::Graph");
        assert(!m_This.expired() && "m_This msut be defined with m_setThis suring the ceration");
        return std::static_pointer_cast<T>(m_This.lock());
    }
    void m_setThis(const GraphWeak &vThis) { m_This = vThis; }

    void m_setNodeThis(NodePtr vNodePtr) {
        if (vNodePtr != nullptr) {
            vNodePtr->m_setThis(vNodePtr);
        }
    }

    template <typename T = GraphDatas>
    explicit Graph(std::shared_ptr<T> vpDatas) : UUID(this), mp_GraphDatas(std::move(vpDatas)) {
        static_assert(std::is_base_of<GraphDatas, T>::value, "T must derive of ez::GraphDatas");
    }

    RetCodes m_addNode(const NodePtr &vNodePtr) {
        auto ret = RetCodes::FAILED_NODE_PTR_NULL;
        if (vNodePtr != nullptr) {
            vNodePtr->m_setThis(vNodePtr);
            vNodePtr->setParentGraph(m_getThis());
            m_Nodes.push_back(vNodePtr);
            m_NodeWeaks.push_back(vNodePtr);
            ret = RetCodes::SUCCESS;
        }
        return ret;
    }

    RetCodes m_delNode(const NodeWeak &vNode) {
        auto ret = RetCodes::FAILED_NODE_NOT_FOUND;
        const auto itShared = Utils::isSharedPtrExistInVector(vNode.lock(), m_Nodes);
        if (itShared != m_Nodes.end()) {
            itShared->get()->unit();
            m_Nodes.erase(itShared);
            auto itWeak = Utils::isWeakPtrExistInVector(vNode, m_NodeWeaks);
            if (itWeak != m_NodeWeaks.end()) {
                m_NodeWeaks.erase(itWeak);
                ret = RetCodes::SUCCESS;
            }
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
        const auto fromPtr = vFrom.lock();
        const auto toPtr = vTo.lock();
        if (fromPtr != nullptr) {
            ret = fromPtr->m_disconnectSlot(vTo);
        }
        if (toPtr != nullptr) {
            ret = toPtr->m_disconnectSlot(vFrom);
        }
        return ret;
    }
};

}  // namespace ez
