#include "TestEzGraph.h"
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezCTest.hpp>

class TestNode;
typedef std::shared_ptr<TestNode> TestNodePtr;
typedef std::weak_ptr<TestNode> TestNodeWeak;

struct TestNodeDatas : ez::NodeDatas {
    std::string mode;
    TestNodeDatas() = default;
    explicit TestNodeDatas(std::string vName, std::string vType, std::string mode) : ez::NodeDatas(std::move(vName), std::move(vType)), mode(std::move(mode)) {}
};

class TestNode : public ez::Node {
public:
    static TestNodePtr create(const TestNodeDatas &vNodeDatas) {
        auto node_ptr = std::make_shared<TestNode>(vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        return node_ptr;
    }

    template <typename T, typename = std::enable_if<std::is_base_of<TestNodeDatas, T>::value>>
    explicit TestNode(const T &vDatas) : Node(vDatas) {}

    template <typename U, typename = std::enable_if<std::is_base_of<ez::Slot, U>::value>>
    std::weak_ptr<U> addSlot(const ez::SlotDatas &vSlotDatas) {
        auto slot_ptr = std::make_shared<U>(vSlotDatas);
        if ((!slot_ptr->init()) || (m_addSlot(slot_ptr) != ez::RetCodes::SUCCESS)) {
            slot_ptr.reset();
        }
        // test null slot for coverage
        m_addSlot(nullptr);
        return slot_ptr;
    }

    template <typename U, typename = std::enable_if<std::is_base_of<ez::Slot, U>::value>>
    ez::RetCodes delSlot(const std::weak_ptr<U> &vSlot) {
        return m_delSlot(vSlot.lock());
    }
};

class TestGraph;
typedef std::shared_ptr<TestGraph> TestGraphPtr;
typedef std::weak_ptr<TestGraph> TestGraphWeak;

struct TestGraphDatas : ez::GraphDatas {
    std::string mode;
    TestGraphDatas() = default;
    explicit TestGraphDatas(std::string vName, std::string vType, std::string mode) : ez::GraphDatas(std::move(vName), std::move(vType)), mode(std::move(mode)) {}
};

class TestGraph : public ez::Graph {
public:
    static TestGraphPtr create(const TestGraphDatas &vNodeDatas) {
        auto graph_ptr = std::make_shared<TestGraph>(vNodeDatas);
        graph_ptr->m_setThis(graph_ptr);
        if (!graph_ptr->init()) {
            graph_ptr.reset();
        }
        return graph_ptr;
    }

    template <typename T, typename = std::enable_if<std::is_base_of<TestGraphDatas, T>::value>>
    explicit TestGraph(const T &vDatas) : Graph(vDatas) {}

    static ez::RetCodes connectSlots(const ez::SlotWeak &vFrom, const ez::SlotWeak &vTo) { 
        return m_connectSlots(vFrom, vTo); }

    template <typename U, typename = std::enable_if<std::is_base_of<ez::Node, U>::value>>
    std::weak_ptr<U> createChildNode(const TestNodeDatas &vNodeDatas) {
        auto node_ptr = U::create(vNodeDatas);
        if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
            node_ptr.reset();
        }
        // test null node for coverage
        m_addNode(nullptr);
        return node_ptr;
    }

    template <typename U, typename = std::enable_if<std::is_base_of<ez::Node, U>::value>>
    ez::RetCodes delNode(const std::weak_ptr<U> &vNode) {
        return m_delNode(vNode.lock());
    }
};

class NodeNumber;
typedef std::shared_ptr<NodeNumber> NodeNumberPtr;
typedef std::weak_ptr<NodeNumber> NodeNumberWeak;

class NodeNumber : public TestNode {
public:
    static NodeNumberPtr create(const TestNodeDatas &vNodeDatas) {
        auto node_ptr = std::make_shared<NodeNumber>(vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

    virtual ez::RetCodes eval(const size_t vFrame, const ez::SlotWeak &vFrom) { return ez::RetCodes::FAILED; }

private:
    float m_Value = 0.0f;

public:
    explicit NodeNumber(const TestNodeDatas &vDatas) : TestNode(vDatas) {}

    void setValue(const float vValue) { m_Value = vValue; }
    float getValue() const { return m_Value; }
};

class NodeOpAdd;
typedef std::shared_ptr<NodeOpAdd> NNodeOpAddPtr;
typedef std::weak_ptr<NodeOpAdd> NodeOpAddWeak;

class NodeOpAdd : public NodeNumber {
public:
    static NNodeOpAddPtr create(const TestNodeDatas &vNodeDatas) {
        auto node_ptr = std::make_shared<NodeOpAdd>(vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

    explicit NodeOpAdd(const TestNodeDatas &vDatas) : NodeNumber(vDatas) {}

    ez::RetCodes eval(const size_t vFrame, const ez::SlotWeak &vFrom) final {
        auto ret = ez::RetCodes::SUCCESS;
        float sum = 0.0f;
        auto outSlotPtr = vFrom.lock();
        if (outSlotPtr != nullptr) {
            auto outNodePtr = std::dynamic_pointer_cast<NodeOpAdd>(outSlotPtr->getParentNode().lock());
            if (outNodePtr != nullptr) {
                for (const auto &inSlotPtr : m_getInputSlotsRef()) {
                    if (inSlotPtr != nullptr) {
                        for (const auto &conSlot : inSlotPtr->m_getConnectedSlots()) {
                            auto conSlotPtr = conSlot.lock();
                            if (conSlotPtr != nullptr) {
                                auto inNodePtr = std::dynamic_pointer_cast<NodeNumber>(conSlotPtr->getParentNode().lock());
                                if (inNodePtr != nullptr) {
                                    sum += inNodePtr->getValue();
                                } else {
                                    ret = ez::RetCodes::FAILED_SLOT_PTR_NULL;
                                    break;
                                }
                            }
                        }
                    }
                }
                outNodePtr->setValue(sum);
                ez::EvalDatas datas;
                datas.frame = vFrame;
                outSlotPtr->setLastEvaluatedDatas(datas);
            } else {
                ret = ez::RetCodes::FAILED_SLOT_PTR_NULL;
            }
        } else {
            ret = ez::RetCodes::FAILED_SLOT_PTR_NULL;
        }
        return ret;
    }
};

////////////////////////////////////////////////////////////////////////////
////  Graph ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGraph_Building() {
    return true;
}

bool TestEzGraph_Evaluation() {
    ez::SlotDatas output_slot_datas;
    output_slot_datas.dir = ez::SlotDir::OUTPUT;
    ez::SlotDatas input_slot_datas;
    input_slot_datas.dir = ez::SlotDir::INPUT;

    auto graphPtr = TestGraph::create(TestGraphDatas("graph", "Graph", "modeA"));
    CTEST_ASSERT(graphPtr != nullptr);
    CTEST_ASSERT(graphPtr->getDatas<TestGraphDatas>().mode == "modeA");
    CTEST_ASSERT(graphPtr->getDatas<ez::GraphDatas>().name == "graph");
    CTEST_ASSERT(graphPtr->getDatas<ez::GraphDatas>().type == "Graph");
    graphPtr->getDatasRef<TestGraphDatas>().mode = "modeABis";
    CTEST_ASSERT(graphPtr->getDatas<TestGraphDatas>().mode == "modeABis");

    auto nodaNumA = graphPtr->createChildNode<NodeNumber>(TestNodeDatas("nodaNumA", "NodaNumber", "modeA"));
    CTEST_ASSERT(nodaNumA.expired() == false);
    CTEST_ASSERT(nodaNumA.lock() != nullptr);
    auto nodeNumASlotOutput = nodaNumA.lock()->addSlot<ez::Slot>(output_slot_datas);
    CTEST_ASSERT(nodeNumASlotOutput.expired() == false);
    CTEST_ASSERT(nodeNumASlotOutput.lock() != nullptr);
    nodaNumA.lock()->setValue(5.0f);
    CTEST_ASSERT(nodaNumA.lock()->getValue() == 5.0f);
    CTEST_ASSERT(nodaNumA.lock()->getDatas<TestNodeDatas>().mode == "modeA");
    CTEST_ASSERT(nodaNumA.lock()->getDatas<ez::NodeDatas>().name == "nodaNumA");
    CTEST_ASSERT(nodaNumA.lock()->getDatas<ez::NodeDatas>().type == "NodaNumber");
    nodaNumA.lock()->getDatasRef<TestNodeDatas>().mode = "modeABis";
    CTEST_ASSERT(nodaNumA.lock()->getDatas<TestNodeDatas>().mode == "modeABis");

    auto nodaNumB = graphPtr->createChildNode<NodeNumber>(TestNodeDatas("nodaNumB", "NodaNumber", "modeB"));
    CTEST_ASSERT(nodaNumB.expired() == false);
    CTEST_ASSERT(nodaNumB.lock() != nullptr);
    auto nodeNumBSlotOutput = nodaNumB.lock()->addSlot<ez::Slot>(output_slot_datas);
    CTEST_ASSERT(nodeNumBSlotOutput.expired() == false);
    CTEST_ASSERT(nodeNumBSlotOutput.lock() != nullptr);
    nodaNumB.lock()->setValue(200.0f);
    CTEST_ASSERT(nodaNumB.lock()->getValue() == 200.0f);
    CTEST_ASSERT(nodaNumB.lock()->getDatas<TestNodeDatas>().mode == "modeB");
    CTEST_ASSERT(nodaNumB.lock()->getDatas<ez::NodeDatas>().name == "nodaNumB");
    CTEST_ASSERT(nodaNumB.lock()->getDatas<ez::NodeDatas>().type == "NodaNumber");
    nodaNumB.lock()->getDatasRef<TestNodeDatas>().mode = "modeBBis";
    CTEST_ASSERT(nodaNumB.lock()->getDatas<TestNodeDatas>().mode == "modeBBis");

    auto nodaOpAdd = graphPtr->createChildNode<NodeOpAdd>(TestNodeDatas("nodaOpAdd", "NodeOpAdd", "modeAdd"));
    CTEST_ASSERT(nodaOpAdd.expired() == false);
    CTEST_ASSERT(nodaOpAdd.lock() != nullptr);
    auto nodeOpAddSlotInputA = nodaOpAdd.lock()->addSlot<ez::Slot>(input_slot_datas);
    CTEST_ASSERT(nodeOpAddSlotInputA.expired() == false);
    CTEST_ASSERT(nodeOpAddSlotInputA.lock() != nullptr);
    auto nodeOpAddSlotInputB = nodaOpAdd.lock()->addSlot<ez::Slot>(input_slot_datas);
    CTEST_ASSERT(nodeOpAddSlotInputB.expired() == false);
    CTEST_ASSERT(nodeOpAddSlotInputB.lock() != nullptr);
    auto nodeOpAddSlotOutput = nodaOpAdd.lock()->addSlot<ez::Slot>(output_slot_datas);
    CTEST_ASSERT(nodeOpAddSlotOutput.expired() == false);
    CTEST_ASSERT(nodeOpAddSlotOutput.lock() != nullptr);
    CTEST_ASSERT(graphPtr->connectSlots(nodeNumASlotOutput.lock(), {}) == ez::RetCodes::FAILED_SLOT_PTR_NULL);
    CTEST_ASSERT(graphPtr->connectSlots({}, nodeOpAddSlotInputB.lock()) == ez::RetCodes::FAILED_SLOT_PTR_NULL);
    CTEST_ASSERT(graphPtr->connectSlots(nodeNumASlotOutput.lock(), nodeOpAddSlotInputA.lock()) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(graphPtr->connectSlots(nodeNumBSlotOutput.lock(), nodeOpAddSlotInputB.lock()) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(nodaOpAdd.lock()->eval(10, nodeOpAddSlotOutput.lock()) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(nodaOpAdd.lock()->getValue() == 205.0f);
    CTEST_ASSERT(nodeOpAddSlotOutput.lock()->getLastEvaluatedDatas().frame == 10);
    CTEST_ASSERT(nodaOpAdd.lock()->getDatas<TestNodeDatas>().mode == "modeAdd");
    CTEST_ASSERT(nodaOpAdd.lock()->getDatas<ez::NodeDatas>().name == "nodaOpAdd");
    CTEST_ASSERT(nodaOpAdd.lock()->getDatas<ez::NodeDatas>().type == "NodeOpAdd");
    nodaOpAdd.lock()->getDatasRef<TestNodeDatas>().mode = "modeAddBis";
    CTEST_ASSERT(nodaOpAdd.lock()->getDatas<TestNodeDatas>().mode == "modeAddBis");

    CTEST_ASSERT(nodaOpAdd.lock()->delSlot(nodeOpAddSlotInputA) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(nodeOpAddSlotInputA.expired() == true);
    CTEST_ASSERT(nodeOpAddSlotInputA.lock() == nullptr);
    CTEST_ASSERT(nodaOpAdd.lock()->delSlot(nodeOpAddSlotInputB) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(nodeOpAddSlotInputB.expired() == true);
    CTEST_ASSERT(nodeOpAddSlotInputB.lock() == nullptr);
    CTEST_ASSERT(nodaOpAdd.lock()->delSlot(nodeOpAddSlotOutput) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(nodeOpAddSlotOutput.expired() == true);
    CTEST_ASSERT(nodeOpAddSlotOutput.lock() == nullptr);

    CTEST_ASSERT(graphPtr->delNode(nodaNumA) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(graphPtr->delNode(nodaNumB) == ez::RetCodes::SUCCESS);
    CTEST_ASSERT(graphPtr->delNode(nodaOpAdd) == ez::RetCodes::SUCCESS);

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

#define IfTestCollectionExist(v, str)        \
    if (vTest.find(#v) != std::string::npos) \
    return v(str)

bool TestEzGraph(const std::string& vTest) {
    IfTestExist(TestEzGraph_Building);
    else IfTestExist(TestEzGraph_Evaluation);

    return false;
}
